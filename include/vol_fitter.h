#pragma once
#include <optional>
#include <string>
#include <cmath>
#include <unordered_map>
#include "json.hpp"
#include "models.h"
#include "interfaces.h"
#include "Poco/Delegate.h"
#include "ceres/ceres.h"
#include "option_map_manager.h"

class VolFitter 
{
private:
	OptionMapManager &omm;
    Models::OptionsMapPtr optionsMap;

    // Struct to hold market data
    struct MarketData {
        double logMoneyness;  // log(K/F)
        double totalVariance; // σ^2 * T
    };

    // SVI model parameters: {a, b, rho, m, sigma}
    // Cost functor for SVI model
    struct SVICostFunctor {
        SVICostFunctor(double logMoneyness, double totalVariance)
            : logMoneyness_(logMoneyness), totalVariance_(totalVariance) {}

        template <typename T>
            bool operator()(const T* const params, T* residual) const {
                const T& a = params[0];
                const T& b = params[1];
                const T& rho = params[2];
                const T& m = params[3];
                const T& sigma = params[4];

                // SVI model
                T svi_variance = a + b * (rho * (logMoneyness_ - m) + sqrt((logMoneyness_ - m) * (logMoneyness_ - m) + sigma * sigma));

                // Compute the residual
                residual[0] = svi_variance - T(totalVariance_);

                return true;
            }

        private:
        const double logMoneyness_;
        const double totalVariance_;
    };

    void onOptionsMapUpdate(const void*, Models::OptionsMapUpdate& newOption);


public:
  VolFitter(OptionMapManager &omm, Models::OptionsMapPtr data);
  ~VolFitter();

  int solve();
};
