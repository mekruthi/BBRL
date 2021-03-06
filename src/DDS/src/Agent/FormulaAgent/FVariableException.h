
#ifndef FVARIABLEEXCEPTION_H
#define FVARIABLEEXCEPTION_H

#include "../../dds.h"
#include "../../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	FVariableException
	\author 	Castronovo Michael
	
	\brief 	Exception.
	\date 	2014-12-13
*/
// ===========================================================================
class dds::FVariableException : public std::exception
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================
		/**
			\brief 		Constructor.
			
			\param[msg	The error message to transmit.
		*/
		FVariableException(std::string msg_);
		
		
		virtual ~FVariableException() throw();


		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief		Returns the error message.
			
			\return		The error message.
		*/
		virtual const char* what() const throw();
	
	
	private:
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
			\brief	The error message.
		*/
		std::string msg;
};

#endif
