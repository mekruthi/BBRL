
#include "Agent/Guez/utils/guez_utils.h"

#include "Agent/RandomAgent.h"
#include "Agent/OptimalAgent.h"
#include "Agent/EAgent.h"
#include "Agent/EGreedyAgent.h"
#include "Agent/SoftMaxAgent.h"
#include "Agent/VDBEEGreedyAgent.h"
#include "Agent/FormulaAgent/FormulaAgent.h"
#include "Agent/BAMCPAgent.h"
#include "Agent/BFS3Agent.h"
#include "Agent/SBOSSAgent.h"
#include "Agent/BEBAgent.h"
#include "Agent/OPPSDSAgent.h"
#include "Agent/OPPSCSAgent.h"
#include "Agent/SLAgent/ANNAgent.h"

#include "Agent/FormulaAgent/QMean.h"
#include "Agent/FormulaAgent/QSelf.h"
#include "Agent/FormulaAgent/QUniform.h"

#include "AgentFactory/EGreedyAgentFactory.h"
#include "AgentFactory/SoftMaxAgentFactory.h"
#include "AgentFactory/VDBEEGreedyAgentFactory.h"
#include "AgentFactory/FormulaAgentFactory.h"
#include "AgentFactory/SLAgentFactory/ANNAgentFactory.h"

#include "MDP/MDP.h"
#include "MDP/CModel.h"

#include "MDPDistribution/DirMultiDistribution.h"

#include "Experiment/Experiment.h"

#include "dds.h"

using namespace std;


// ===========================================================================
//	Functions
// ===========================================================================
void dds::init(unsigned int seed)
{
	//	Initialize the RNGs
	if (seed == 0) { utils::RandomGen::setSeed(time(0)); }
	else           { utils::RandomGen::setSeed(seed);    }
	guez_utils::setSeed(utils::RandomGen::randIntRange_Uniform(0, INT_MAX));
	
	
	//	Agents
	Serializable::checkIn<RandomAgent>(
			&Serializable::createInstance<RandomAgent>);

	Serializable::checkIn<OptimalAgent>(
			&Serializable::createInstance<OptimalAgent>);

     Serializable::checkIn<EAgent>(
			&Serializable::createInstance<EAgent>);

	Serializable::checkIn<EGreedyAgent>(
			&Serializable::createInstance<EGreedyAgent>);

	Serializable::checkIn<SoftMaxAgent>(
			&Serializable::createInstance<SoftMaxAgent>);

	Serializable::checkIn<VDBEEGreedyAgent>(
			&Serializable::createInstance<VDBEEGreedyAgent>);

     Serializable::checkIn<FormulaAgent>(
			&Serializable::createInstance<FormulaAgent>);

     Serializable::checkIn<BAMCPAgent>(
			&Serializable::createInstance<BAMCPAgent>);

     Serializable::checkIn<BFS3Agent>(
			&Serializable::createInstance<BFS3Agent>);

     Serializable::checkIn<SBOSSAgent>(
			&Serializable::createInstance<SBOSSAgent>);

     Serializable::checkIn<BEBAgent>(
			&Serializable::createInstance<BEBAgent>);

	Serializable::checkIn<OPPSDSAgent>(
			&Serializable::createInstance<OPPSDSAgent>);

	Serializable::checkIn<OPPSCSAgent>(
			&Serializable::createInstance<OPPSCSAgent>);

     Serializable::checkIn<ANNAgent>(
               &Serializable::createInstance<ANNAgent>);


     //   FVariables
     Serializable::checkIn<QMean>(
			&Serializable::createInstance<QMean>);

     Serializable::checkIn<QSelf>(
			&Serializable::createInstance<QSelf>);

     Serializable::checkIn<QUniform>(
			&Serializable::createInstance<QUniform>);

     Serializable::checkIn<QCounterVar>(
			&Serializable::createInstance<QCounterVar>);
		
			
	//  AgentFactorys
	Serializable::checkIn<EGreedyAgentFactory>(
			&Serializable::createInstance<EGreedyAgentFactory>);

	Serializable::checkIn<SoftMaxAgentFactory>(
			&Serializable::createInstance<SoftMaxAgentFactory>);

	Serializable::checkIn<VDBEEGreedyAgentFactory>(
			&Serializable::createInstance<VDBEEGreedyAgentFactory>);

	Serializable::checkIn<FormulaAgentFactory>(
			&Serializable::createInstance<FormulaAgentFactory>);

     Serializable::checkIn<ANNAgentFactory>(
			&Serializable::createInstance<ANNAgentFactory>);


	//	MDP
	Serializable::checkIn<MDP>(
			&Serializable::createInstance<MDP>);

	
	//	Models
	Serializable::checkIn<CModel>(
			&Serializable::createInstance<CModel>);


	//	Distributions
	Serializable::checkIn<DirMultiDistribution>(
			&Serializable::createInstance<DirMultiDistribution>);


	//	Experiments
	Serializable::checkIn<Experiment>(
			&Serializable::createInstance<Experiment>);
}


// ---------------------------------------------------------------------------
//	'simulation' namespace
// ---------------------------------------------------------------------------
// ===========================================================================
//	Functions
// ===========================================================================
dds::simulation::SimulationRecord dds::simulation::simulate(
		dds::Agent* agent, dds::MDP* mdp,
		double gamma, unsigned int T,
		bool safeSim) throw (AgentException, MDPException)
{
	assert(agent);
	assert(mdp);
	assert((gamma > 0.0) && (gamma <= 1.0));


	//	Create a SimulationRecord
	dds::simulation::SimulationRecord simRec(gamma);
	
	
	//	Reset the MDP
	mdp->reset();
	
	
	//	Set the MDP as 'unknown'
	if (safeSim) { mdp->setUnknown(); }
 
	
	//	Initialization	
	unsigned int x = mdp->getCurrentState();
	agent->setMDP(mdp, gamma, T);
	
	
	//	Simulation
	for (unsigned int t = 0; t < T; ++t)
	{
		//	Retrieve the action to perform
		unsigned int u = agent->getAction(x);
		
		
		//	Perform the selected action
		unsigned int y;
		double r;
		mdp->perform(u, y, r);
		
		
		//	Store the Transition observed
		simRec.add(Transition(x, u, y, r));
		
		
		//	Online learning of the agent
		agent->learnOnline(x, u, y, r);
		
		
		//	Update of the data
		x = y;
	}
	
	
	//	Set the MDP as 'known'
	if (safeSim) { mdp->setKnown(); }
	
	
	//   Free unnecessary data
	agent->freeData();
	
	
	//	Return
	return simRec;
}


// ---------------------------------------------------------------------------
//	'opps' namespace
// ---------------------------------------------------------------------------
dds::opps::StoSOO::StoSOO(
		unsigned int K, unsigned int k, unsigned int hMax, double delta,
		AgentFactory* agentFactory_,
		const MDPDistribution* mdpDistrib_,
		double gamma_, unsigned int T_) :
				utils::algorithm::StoSOO(
                         agentFactory_->getNbParam(),
                         K, k, hMax, delta, agentFactory_->getBounds()),
				agentFactory(agentFactory_), mdpDistrib(mdpDistrib_),
				gamma(gamma_), T(T_)
{
	assert(mdpDistrib);
	assert((gamma > 0.0) && (gamma <= 1.0));
}


double dds::opps::StoSOO::f(const std::vector<double>& x) const
                                                       throw (std::exception)
{
//      cout << "\n\n\t[ Sto-SOO testing < " << x[0];
//      for (unsigned int i = 1; i < x.size(); ++i) { cout << ", " << x[i]; }
//      cout << " > ]\n";
// 
//      Agent* agent = agentFactory->get(x);
// 	MDP* mdp = mdpDistrib->draw();
// 		
// 	dds::simulation::SimulationRecord simRec;
// 	simRec = dds::simulation::simulate(agent, mdp, gamma, T, false);
// 	
// 	delete agent;
// 	delete mdp;
// 	
// 	double dsr = simRec.computeDSR();
// 	cout << "\n\t[ Sto-SOO got " << dsr << " ]\n\n";
// 	
// 	return dsr;

     unsigned int nMDPs = 500;

     cout << "\n\n\t[ Sto-SOO testing < " << x[0];
     for (unsigned int i = 1; i < x.size(); ++i) { cout << ", " << x[i]; }
     cout << " > ]\n";

     Agent* agent = agentFactory->get(x);
	
	double meanDSR = 0.0;
     for (unsigned int i = 0; i < nMDPs; ++i)
     {
          MDP* mdp = mdpDistrib->draw();
          
          dds::simulation::SimulationRecord simRec;
          simRec = dds::simulation::simulate(agent, mdp, gamma, T, false);
          
          delete mdp;
     
          meanDSR += simRec.computeDSR();
	}
	meanDSR /= nMDPs;
	
	delete agent;
	
	cout << "\n\t[ Sto-SOO got " << meanDSR << " ]\n\n";
	return meanDSR;
}


dds::opps::UCB1::UCB1(
		double c,
		const vector<Agent*>& strategyList_,
		const MDPDistribution* mdpDistrib_,
		double gamma_, unsigned int T_) :
				utils::algorithm::UCB1(strategyList_.size(), c),
				strategyList(strategyList_), mdpDistrib(mdpDistrib_),
				gamma(gamma_), T(T_)
{
	assert(!strategyList.empty());
	assert(mdpDistrib);
	assert((gamma > 0.0) && (gamma <= 1.0));
}


double dds::opps::UCB1::drawArm(unsigned int i) const throw (std::exception)
{
	MDP* mdp = mdpDistrib->draw();
	
	dds::simulation::SimulationRecord simRec;
	simRec = dds::simulation::simulate(strategyList[i], mdp, gamma, T, false);
	
	delete mdp;
	
	return simRec.computeDSR();
}


dds::opps::UCT::UCT(
		double c,
		AgentFactory* agentFactory_,
		const MDPDistribution* mdpDistrib_,
		double gamma_, unsigned int T_) :
				utils::algorithm::UCT(c, agentFactory_->getBounds(),
							       agentFactory_->getSplitAcc()),
				agentFactory(agentFactory_), mdpDistrib(mdpDistrib_),
				gamma(gamma_), T(T_)
{
	assert(mdpDistrib);
	assert((gamma > 0.0) && (gamma <= 1.0));
}


double dds::opps::UCT::drawArm(
		const vector<double>& paraSList) const throw (std::exception)
{
	Agent* agent = agentFactory->get(paraSList);
	MDP* mdp = mdpDistrib->draw();
		
	dds::simulation::SimulationRecord simRec;
	simRec = dds::simulation::simulate(agent, mdp, gamma, T, false);
	
	delete agent;
	delete mdp;
	
	return simRec.computeDSR();
}
