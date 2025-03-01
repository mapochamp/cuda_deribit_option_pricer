#include "vol_fitter.h"
#include <chrono>
#include <ctime>

VolFitter::VolFitter(OptionMapManager &omm, Models::OptionsMapPtr data)
    : omm(omm), optionsMap(data), fitted_params(5, 0.0)  // Initialize fitted_params with 5 zeros
{
    omm.optionsMapUpdate += Poco::delegate(this, &VolFitter::onOptionsMapUpdate);
}

VolFitter::~VolFitter()
{
    omm.optionsMapUpdate -= Poco::delegate(this, &VolFitter::onOptionsMapUpdate);
}

float VolFitter::calculate_time_to_expiry(int expiration) {
    // Convert YYYYMMDD format to time_t
    std::tm exp_tm = {};
    exp_tm.tm_year = expiration / 10000 - 1900;  // Extract year and adjust for tm format
    exp_tm.tm_mon = (expiration % 10000) / 100 - 1;  // Extract month (0-11)
    exp_tm.tm_mday = expiration % 100;  // Extract day
    
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    
    // Convert expiration to time_t
    auto exp_time_t = std::mktime(&exp_tm);
    
    // Calculate difference in years
    double seconds_diff = std::difftime(exp_time_t, now_time_t);
    return static_cast<float>(seconds_diff / (365.25 * 24 * 60 * 60));  // Convert to years
}

float VolFitter::get_svi_vol(float K, float F, float T, float a, float b, float rho, float m, float sigma)
{
    float k = log(K/F);
    float w = a + b * (rho * (k - m) + std::sqrt(std::pow((k - m),2) + std::pow(sigma,2)));
    float svi_vol = std::sqrt(w / T);
    return svi_vol;
}

void VolFitter::onOptionsMapUpdate(const void*, Models::OptionsMapUpdate& newOption)
{
    // Get all options for this expiry
    auto& expiry_map = (*optionsMap)[newOption.expiration];
    std::vector<MarketData> market_data;

    // Forward price - assuming we have this from somewhere
    float F = 1.0; // TODO: Get actual forward price
    float T = calculate_time_to_expiry(newOption.expiration);

    // Collect market data for both calls and puts at this expiry
    for (const auto& [strike, option] : expiry_map[Models::OptionType::call]) {
        if (option.mark_iv > 0) {  // Only use valid IVs
            float k = log(float(strike)/F);  // Log-moneyness
            float w = std::pow(option.mark_iv, 2) * T;  // Total variance
            market_data.push_back({k, w});
        }
    }

    if (market_data.empty()) {
        return;  // Not enough data to fit
    }

    solve(market_data);
    
    // Notify GUI of update with proper event object
    UpdateEvent event;
    volFitterUpdate.notify(this, event);
}


int VolFitter::solve(const std::vector<MarketData>& market_data)
{
    // Initial guess for SVI parameters {a, b, rho, m, sigma}
    double parameters[5] = {0.04, 0.4, -0.4, 0.0, 0.1};

    // Build the problem
    ceres::Problem problem;

    for (const auto& data : market_data) {
        ceres::CostFunction* cost_function =
            new ceres::AutoDiffCostFunction<SVICostFunctor, 1, 5>(
                new SVICostFunctor(data.logMoneyness, data.totalVariance));

        problem.AddResidualBlock(cost_function, nullptr, parameters);
    }

    // Add bounds to parameters
    problem.SetParameterLowerBound(parameters, 0, 0.0);  // a > 0
    problem.SetParameterLowerBound(parameters, 1, 0.0);  // b > 0
    problem.SetParameterLowerBound(parameters, 2, -1.0); // -1 < rho < 1
    problem.SetParameterUpperBound(parameters, 2, 1.0);
    problem.SetParameterLowerBound(parameters, 4, 0.0);  // sigma > 0

    // Set up the solver
    ceres::Solver::Options options;
    options.linear_solver_type = ceres::DENSE_QR;
    options.minimizer_progress_to_stdout = true;
    options.max_num_iterations = 100;

    // Solve
    ceres::Solver::Summary summary;
    ceres::Solve(options, &problem, &summary);

    // Store the fitted parameters
    fitted_params = {
        parameters[0], parameters[1], parameters[2], 
        parameters[3], parameters[4]
    };

    std::cout << "SVI Fit Results for expiry: " << std::endl;
    std::cout << "a: " << parameters[0] << std::endl;
    std::cout << "b: " << parameters[1] << std::endl;
    std::cout << "rho: " << parameters[2] << std::endl;
    std::cout << "m: " << parameters[3] << std::endl;
    std::cout << "sigma: " << parameters[4] << std::endl;
    std::cout << "Cost: " << summary.final_cost << std::endl;

    return 0;
}
