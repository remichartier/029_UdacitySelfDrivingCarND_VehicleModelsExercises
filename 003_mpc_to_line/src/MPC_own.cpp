#include "MPC.h"
#include <math.h>
#include <cppad/cppad.hpp>

#define HAVE_CSTDDEF
#include <cppad/ipopt/solve.hpp>
#undef HAVE_CSTDDEF

#include <vector>
#include "Eigen-3.3/Eigen/Core"

using CppAD::AD;
using Eigen::VectorXd;

/**
 * TODO: Set N and dt
 */
/*size_t N = ? ;
double dt = ? ;
*/
size_t N = 25;
double dt = 0.5;
// This value assumes the model presented in the classroom is used.
//
// It was obtained by measuring the radius formed by running the vehicle in the
// simulator around in a circle with a constant steering angle and velocity on a
// flat terrain.
//
// Lf was tuned until the the radius formed by the simulating the model
// presented in the classroom matched the previous radius.
//
// This is the length from front to CoG that has a similar radius.
const double Lf = 2.67;

// NOTE: feel free to play around with this or do something completely different
double ref_v = 40;

// The solver takes all the state variables and actuator
// variables in a singular vector. Thus, we should to establish
// when one variable starts and another ends to make our lifes easier.
size_t x_start = 0;
size_t y_start = x_start + N;
size_t psi_start = y_start + N;
size_t v_start = psi_start + N;
size_t cte_start = v_start + N;
size_t epsi_start = cte_start + N;
size_t delta_start = epsi_start + N;
size_t a_start = delta_start + N - 1;

class FG_eval {
 public:
  VectorXd coeffs;
  // Coefficients of the fitted polynomial.
  FG_eval(VectorXd coeffs) { this->coeffs = coeffs; }

  typedef CPPAD_TESTVECTOR(AD<double>) ADvector;
  // `fg` is a vector containing the cost and constraints.
  // `vars` is a vector containing the variable values (state & actuators).

  /* Here in operator() you'll define the cost function and constraints. 
  	x is already completed
  */
  void operator()(ADvector& fg, const ADvector& vars) {
    // The cost is stored is the first element of `fg`.
    // Any additions to the cost should be added to `fg[0]`.

    /*  fg[0] stores the cost value, so the fg vector is 1 element larger 
    than it was in MPC::Solve.
    */
    fg[0] = 0;

    // Reference State Cost
    /**
     * TODO: Define the cost related the reference state and
     *   anything you think may be beneficial.
     */
	for (int t = 0; t < N; ++t) {
	  // Minimize errors : 
	  fg[0] += CppAD::pow(vars[cte_start + t], 2);
	  fg[0] += CppAD::pow(vars[epsi_start + t], 2);
	  // Minimize Cost dealing with stopping
	  fg[0] += CppAD::pow(vars[v_start + t] - ref_v, 2);
	}

	for (int t = 0; t < N-1; ++t) {
	  // Minimize Cost of control input minimize it
	  fg[0] += CppAD::pow(vars[delta_start + t], 2);
	  fg[0] += CppAD::pow(vars[a_start + t], 2);

	  // Minimize Cost of change rate of the control input
	  fg[0] += CppAD::pow(vars[delta_start + t+1] - vars[delta_start + t], 2);
	  fg[0] += CppAD::pow(vars[a_start + t+1] - vars[a_start + t], 2);
	}


    //
    // Setup Constraints
    //
    // NOTE: In this section you'll setup the model constraints.

    // Initial constraints
    //
    // We add 1 to each of the starting indices due to cost being located at
    // index 0 of `fg`.
    // This bumps up the position of all the other values.
    fg[1 + x_start] = vars[x_start];
    fg[1 + y_start] = vars[y_start];
    fg[1 + psi_start] = vars[psi_start];
    fg[1 + v_start] = vars[v_start];
    fg[1 + cte_start] = vars[cte_start];
    fg[1 + epsi_start] = vars[epsi_start];

    /* Note that we start the loop at t=1, because the values at t=0 are set
     to our initial state - those values are not calculated by the solver.
    */
    // The rest of the constraints
    for (int t = 1; t < N; ++t) {
      /**
       * TODO: Grab the rest of the states at t+1 and t.
       *   We have given you parts of these states below.
       */
      AD<double> x1 = vars[x_start + t];
      AD<double> y1 = vars[y_start + t];
      AD<double> psi1 = vars[psi_start + t];
      AD<double> v1 = vars[v_start + t];
      AD<double> cte1 = vars[cte_start + t];
      AD<double> epsi1 = vars[epsi_start + t];

      AD<double> x0 = vars[x_start + t - 1];
      AD<double> y0 = vars[y_start + t - 1];
      AD<double> psi0 = vars[psi_start + t - 1];
      AD<double> v0 = vars[v_start + t - 1];
      AD<double> delta0 = vars[delta_start + t - 1];
      AD<double> a0 = vars[a_start + t - 1];
      AD<double> cte0 = vars[cte_start + t - 1];
      AD<double> epsi0 = vars[epsi_start + t];

	  AD<double> f0 = coeffs[0] + coeffs[1] * x0;
      AD<double> psides0 = CppAD::atan(coeffs[1]);
      // Here's `x` to get you started.
      // The idea here is to constraint this value to be 0.
      //
      // NOTE: The use of `AD<double>` and use of `CppAD`!
      // CppAD can compute derivatives and pass these to the solver.

      /**
       * TODO: Setup the rest of the model constraints
       */
      // Recall the equations for the model:
      // x_[t+1] = x[t] + v[t] * cos(psi[t]) * dt
      // y_[t+1] = y[t] + v[t] * sin(psi[t]) * dt
      // psi_[t+1] = psi[t] + v[t] / Lf * delta[t] * dt
      // v_[t+1] = v[t] + a[t] * dt
      // cte[t+1] = f(x[t]) - y[t] + v[t] * sin(epsi[t]) * dt
      // epsi[t+1] = psi[t] - psides[t] + v[t] * delta[t] / Lf * dt
      fg[1 + x_start + t] = x1 - (x0 + v0 * CppAD::cos(psi0) * dt);
      fg[1 + y_start + t] = y1 - (y0 + v0 * CppAD::sin(psi0) * dt);
      fg[1 + psi_start + t] = psi1 - (psi0 + v0 * delta0 * dt / Lf);
      fg[1 + v_start + t] = v1 - (v0 + a0 * dt); 
      fg[1 + cte_start + t] = cte1 - ((f0 - y0) + v0 * CppAD::sin(epsi0) * dt);
      fg[1 + epsi_start + t] = epsi1 - ((psi0 - psides0) + v0 * delta0 * dt/ Lf);

    }
  }
};

//
// MPC class definition
//

MPC::MPC() {}
MPC::~MPC() {}

/* Personal notes : 
MPC::Solve
x0 is the initial state [x ,y , \psi, v, cte, e\psi][x,y,ψ,v,cte,eψ], coeffs 
are the coefficients of the fitting polynomial. The bulk of this method is 
setting up the vehicle model constraints (constraints) and variables (vars)
 for Ipopt.

 Note Ipopt expects all the constraints and variables as vectors. For 
 example, suppose N is 5, then the structure of vars a 38-element vector:
vars[0],...,vars[4] -> [x_1, ...., x_5][x1,....,x 5]
vars[5],...,vars[9] -> [y_1, ...., y_5][y1,....,y5]
etc ...
*/
std::vector<double> MPC::Solve(const VectorXd &x0, const VectorXd &coeffs) {
  typedef CPPAD_TESTVECTOR(double) Dvector;

  double x = x0[0];
  double y = x0[1];
  double psi = x0[2];
  double v = x0[3];
  double cte = x0[4];
  double epsi = x0[5];

  // number of independent variables
  // N timesteps == N - 1 actuations
  // (N - 1) * 2 for Alpha and a for (N-1) actuations

  size_t n_vars = N * 6 + (N - 1) * 2;
  // Number of constraints
  size_t n_constraints = N * 6;

  // Initial value of the independent variables.
  // Should be 0 except for the initial values.
  Dvector vars(n_vars);
  for (int i = 0; i < n_vars; ++i) {
    vars[i] = 0.0;
  }
  // Set the initial variable values
  vars[x_start] = x;
  vars[y_start] = y;
  vars[psi_start] = psi;
  vars[v_start] = v;
  vars[cte_start] = cte;
  vars[epsi_start] = epsi;

  // Lower and upper limits for x
  Dvector vars_lowerbound(n_vars);
  Dvector vars_upperbound(n_vars);

  // Set all non-actuators upper and lowerlimits
  // to the max negative and positive values.
  for (int i = 0; i < delta_start; ++i) {
    vars_lowerbound[i] = -1.0e19;
    vars_upperbound[i] = 1.0e19;
  }

  // The upper and lower limits of delta are set to -25 and 25
  // degrees (values in radians).
  // NOTE: Feel free to change this to something else.
  for (int i = delta_start; i < a_start; ++i) {
    vars_lowerbound[i] = -0.436332;
    vars_upperbound[i] = 0.436332;
  }

  // Acceleration/decceleration upper and lower limits.
  // NOTE: Feel free to change this to something else.
  for (int i = a_start; i < n_vars; ++i) {
    vars_lowerbound[i] = -1.0;
    vars_upperbound[i] = 1.0;
  }

  // Lower and upper limits for constraints
  // All of these should be 0 except the initial
  // state indices.
  Dvector constraints_lowerbound(n_constraints);
  Dvector constraints_upperbound(n_constraints);
  for (int i = 0; i < n_constraints; ++i) {
    constraints_lowerbound[i] = 0;
    constraints_upperbound[i] = 0;
  }
  constraints_lowerbound[x_start] = x;
  constraints_lowerbound[y_start] = y;
  constraints_lowerbound[psi_start] = psi;
  constraints_lowerbound[v_start] = v;
  constraints_lowerbound[cte_start] = cte;
  constraints_lowerbound[epsi_start] = epsi;

  constraints_upperbound[x_start] = x;
  constraints_upperbound[y_start] = y;
  constraints_upperbound[psi_start] = psi;
  constraints_upperbound[v_start] = v;
  constraints_upperbound[cte_start] = cte;
  constraints_upperbound[epsi_start] = epsi;

  // Object that computes objective and constraints
  /* Personal notes : 
  The FG_eval class has the constructor:

	FG_eval(Eigen::VectorXd coeffs) { this->coeffs = coeffs; }
	where coeffs are the coefficients of the fitted polynomial. coeffs will be 
	used by the cross track error and heading error equations.

	The FG_eval class has only one method:

	void operator()(ADvector& fg, const ADvector& vars)
	vars is the vector of variables (from the previous section) and fg is the 
	vector of constraints.

	One complication: fg[0] stores the cost value, so the fg vector is 1 element
	 larger than it was in MPC::Solve.
  */
  FG_eval fg_eval(coeffs);
  /* this object is then used by lpopt to find the lowest cost trajectory
  */

  // options
  std::string options;
  options += "Integer print_level  0\n";
  options += "Sparse  true        forward\n";
  options += "Sparse  true        reverse\n";


  /* An FG_eval object is created in MPC::Solve:

	FG_eval fg_eval(coeffs);
	This is then used by Ipopt to find the lowest cost trajectory:
  */
  // place to return solution
  CppAD::ipopt::solve_result<Dvector> solution;

  // solve the problem
  CppAD::ipopt::solve<Dvector, FG_eval>(
      options, vars, vars_lowerbound, vars_upperbound, constraints_lowerbound,
      constraints_upperbound, fg_eval, solution);
  /* The filled in vars vector is stored as solution.x and the cost as 
  	solution.obj_value.
  */

  //
  // Check some of the solution values
  //
  bool ok = true;
  ok &= solution.status == CppAD::ipopt::solve_result<Dvector>::success;

  auto cost = solution.obj_value;
  std::cout << "Cost " << cost << std::endl;
  return {solution.x[x_start + 1],   solution.x[y_start + 1],
          solution.x[psi_start + 1], solution.x[v_start + 1],
          solution.x[cte_start + 1], solution.x[epsi_start + 1],
          solution.x[delta_start],   solution.x[a_start]};
}