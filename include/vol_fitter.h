#pragma once
#include <vector>
#include <ceres/ceres.h>
#include "option_map_manager.h"
#include <Poco/Delegate.h>
#include <Poco/BasicEvent.h>

class VolFitter 
{
public:
struct MarketData {
    float logMoneyness;
    float totalVariance;
};

    VolFitter(OptionMapManager &omm, Models::OptionsMapPtr data);
    ~VolFitter();

    int solve(const std::vector<MarketData>& market_data);
    float get_svi_vol(float K, float F, float T, float a, float b, float rho, float m, float sigma);
    float calculate_time_to_expiry(int expiration);
    
    const std::vector<double>& get_fitted_params() const { return fitted_params; }
    
    // Signal for GUI updates - using a simple class as event parameter
    struct UpdateEvent { };
    Poco::BasicEvent<UpdateEvent> volFitterUpdate;

private:
    OptionMapManager &omm;
    Models::OptionsMapPtr optionsMap;
    std::vector<double> fitted_params;

struct SVICostFunctor {
    SVICostFunctor(double k, double w) : k_(k), w_(w) {}

    template <typename T>
    bool operator()(const T* const params, T* residual) const {
        // params = {a, b, rho, m, sigma}
        T k_diff = k_ - params[3];  // k - m
        T w = params[0] + params[1] * (params[2] * k_diff + sqrt(k_diff * k_diff + params[4] * params[4]));
        residual[0] = w - w_;
        return true;
    }

private:
    const double k_;
    const double w_;
};

    void onOptionsMapUpdate(const void*, Models::OptionsMapUpdate& newOption);
};
