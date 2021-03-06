
#ifndef SOFTMAXAGENTFACTORY_H
#define SOFTMAXAGENTFACTORY_H

#include "AgentFactory.h"

#include "../Agent/SoftMaxAgent.h"
#include "../MDP/CModel.h"
#include "../MDPDistribution/DirMultiDistribution.h"

#include "../dds.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	SoftMaxAgentFactory
	\author 	Castronovo Michael
	
	\brief 	An AgentFactory which generates SoftMaxAgent's.
	
	          'init()' must be called after this AgentFactory has been
	          deserialized.

	\date 	2015-02-26
*/
// ===========================================================================
/* final */ class dds::SoftMaxAgentFactory : public dds::AgentFactory
{		
	public:
		// =================================================================
		//	Public Constructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the SoftMaxAgentFactory to load.
					(can either be compressed or uncompressed)
		*/
		SoftMaxAgentFactory(std::istream& is);
		
		
		/**
			\brief		Constructor.
			
			\param[minTau_		The minimal value of tau.
			\param[maxTau_		The maximal value of tau.
		*/
		SoftMaxAgentFactory(double minTau_, double maxTau_);
		
		
		/**
               \brief    Destructor.
		*/
		~SoftMaxAgentFactory();
		
		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "SoftMaxAgentFactory"; }
		
		
		// =================================================================
		//	Public methods
		// =================================================================
		/**
               \brief    Return the name of this AgentFactory.
               
               \return   The name of this AgentFactory.
		*/
		std::string getName() const { return "Soft-max space"; }


		/**
			\brief			Initialize this AgentFactory with respect
							to the given MDP distribution.
			
			                    Must be called after this AgentFactory has
							been deserialized.
			
			\param[mdpDistrib	A MDP distribution.
		*/
		void init(const MDPDistribution* mdpDistrib)
									throw (AgentFactoryException);


		/**
			\brief			Return an agent parametrized by 'paramList'.
			
			\param[paramList	The parameters to use.
			
			\return			An agent parametrized by 'paramList'.
		*/
		Agent* get(const std::vector<double>& paramList) const
									throw (AgentFactoryException);

		
		/**
			\brief			Return the list of bounds on each parameter.
			
			\return			The list of bounds on each parameter.
		*/
		std::vector<std::pair<double, double> > getBounds() const
									throw (AgentFactoryException)
		{
			return boundsList;
		}


		/**
			\brief			Return the list of split accuracies of each
							parameter.
			
			\return			The list of split accuracies of each
							parameter.
		*/
		std::vector<double> getSplitAcc() const throw (AgentFactoryException)
		{
			return splitAccList;
		}
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const
		{
		   return SoftMaxAgentFactory::toString();
		}

		
		/**
			\brief	Serialize this Object.
					If overloaded, the new 'serialize()' method should
					call the 'serialize()' method of the base class
					before doing anything else.
		*/
		void serialize(std::ostream& os) const;
		
		
		/**
			\brief	Deserialize this Object.
					If overloaded, the new 'deserialize()' method should
					call the 'deserialize()' method of the base class
					before doing anything else.
		*/
		void deserialize(std::istream& is) throw (SerializableException);


	private:
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
			\brief	The minimal and maximal values of tau respectively.
		*/
		double minTau, maxTau;


          /**
               \brief    The initial model used by the SoftMaxAgent generated.
		*/
		CModel* iniModel;

		
		/**
			\brief	The list of bounds on each parameter.
		*/
		std::vector<std::pair<double, double> > boundsList;
		
		
		/**
			\brief	The list of split accuracies of each parameter.
		*/
		std::vector<double> splitAccList;
};

#endif
