#include "vol_fitter.h"

VolFitter::VolFitter(OptionMapManager &omm, Models::OptionsMapPtr data)
    : omm(omm), optionsMap(data)
{
    omm.optionsMapUpdate += Poco::delegate(this, &VolFitter::onOptionsMapUpdate);
}

VolFitter::~VolFitter()
{
    omm.optionsMapUpdate -= Poco::delegate(this, &VolFitter::onOptionsMapUpdate);
}

// TODO: we need to create a map of fitted SVI params per expiry
//       since we are fitting a seperate curve per expiry.

void VolFitter::onOptionsMapUpdate(const void*, Models::OptionsMapUpdate& newOption)
{
    solve();
}

int VolFitter::solve()
{
    std::vector<MarketData> market_data = {
        {-0.1, 0.04},
        {-0.05, 0.0361},
        {0, 0.0324},
        {0.05, 0.0361},
        {0.1, 0.04}
    };

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

    // Print results
    std::cout << summary.BriefReport() << "\n";
    std::cout << "Initial parameters: 0.04, 0.4, -0.4, 0.0, 0.1\n";
    std::cout << "Final   parameters: " 
              << parameters[0] << ", " 
              << parameters[1] << ", " 
              << parameters[2] << ", " 
              << parameters[3] << ", " 
              << parameters[4] << "\n";

    return 0;
}
