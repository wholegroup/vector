#pragma once

#ifndef VECTOR_PS_SUITE_H
#define VECTOR_PS_SUITE_H

template<class T> class Suite
{
	private:

		// Suite name
		char const * const name;

		// Suite version
		int32 const version;

		// Basic suite
		SPBasicSuite * const sSPBasic;

		// Reference to suit
		T * suite;

		// Error code
		SPErr error;

	public:

		Suite(char const * const name, int32 const version, SPBasicSuite * const sSPBasic)
			: name(name), 
			version(version), 
			suite(nullptr), 
			sSPBasic(sSPBasic),
			error(kSPNoError)
		{
			if (nullptr != sSPBasic)
			{
				error = sSPBasic->AcquireSuite(name, version, 
					const_cast<const void **>(reinterpret_cast<void **>(&suite)));
			}
			else
			{
				error = kSPBadParameterError;
			}
		}

		~Suite()
		{
			if ((nullptr != suite) && (nullptr != sSPBasic) && (nullptr != name))
			{
				sSPBasic->ReleaseSuite(name, version);
			}
		}

		const T * operator->() const
		{ 
			return this->suite; 
		}

		const T * operator *() const
		{
			return this->suite;
		}

		SPErr getError() const {
			return error;
		}
};

template<class T>
bool operator!=(void * const lhs, Suite<T> const & rhs)
{
	return lhs != *rhs;
}

template<class T>
bool operator==(void * const lhs, Suite<T> const & rhs)
{
	return lhs == *rhs;
}

typedef Suite<PSActionDescriptorProcs> ActionDescriptorSuite;
typedef Suite<PSActionControlProcs> ActionControlSuite;
typedef Suite<PSActionReferenceProcs> ActionReferenceSuite;
typedef Suite<PSHandleSuite2> HandleSuite;
typedef Suite<PSBufferSuite1> BufferSuite;
typedef Suite<PSProgressSuite1> ProgressSuite;
typedef Suite<ASZStringSuite> StringSuite;
typedef Suite<PropertyProcs> PropertySuite;

#define CreateActionDescriptorSuite(sSPBasic) \
	ActionDescriptorSuite(kPSActionDescriptorSuite, kPSActionDescriptorSuiteVersion, sSPBasic)

#define CreateActionDescriptorSuitePrev(sSPBasic) \
	ActionDescriptorSuite(kPSActionDescriptorSuite, kPSActionDescriptorSuitePrevVersion, sSPBasic)

#define CreateActionControlSuite(sSPBasic) \
	ActionControlSuite(kPSActionControlSuite, kPSActionControlSuiteVersion, sSPBasic)

#define CreateActionControlSuitePrev(sSPBasic) \
	ActionControlSuite(kPSActionControlSuite, kPSActionControlSuitePrevVersion, sSPBasic)

#define CreateActionReferenceSuite(sSPBasic) \
	ActionReferenceSuite(kPSActionReferenceSuite, kPSActionReferenceSuiteVersion, sSPBasic)

#define CreateActionReferenceSuitePrev(sSPBasic) \
	ActionReferenceSuite(kPSActionReferenceSuite, kPSActionReferenceSuitePrevVersion, sSPBasic)

#define CreateHandleSuite(sSPBasic) \
	HandleSuite(kPSHandleSuite, kPSHandleSuiteVersion2, sSPBasic)

#define CreateBufferSuite(sSPBasic) \
	BufferSuite(kPSBufferSuite, kPSBufferSuiteVersion1, sSPBasic)

#define CreateProgressSuite(sSPBasic) \
	ProgressSuite(kPSProgressSuite, kPSProgressSuiteVersion1, sSPBasic)

#define CreateStringSuite(sSPBasic) \
	StringSuite(kASZStringSuite, kASZStringSuiteVersion1, sSPBasic)

#define CreatePropertySuite(sSPBasic) \
	PropertySuite(kPIPropertySuite, kPIPropertySuiteVersion, sSPBasic)

class Auto_Ref
{
	private:

		PIActionReference reference;

		ActionReferenceSuite const & actionReferenceSuite;

	public:

		Auto_Ref(ActionReferenceSuite const & actionReferenceSuite, bool make = true)
			: reference(nullptr), actionReferenceSuite(actionReferenceSuite)
		{
			if (make && (nullptr != actionReferenceSuite))
			{
				actionReferenceSuite->Make(&reference);
			}
		}

		~Auto_Ref()
		{
			if (nullptr != reference)
			{
				actionReferenceSuite->Free(reference);
			}
		}

		PIActionReference get() const
		{
			return reference; 
		}

	/*
	PIActionReference * operator & ()
	{
		return (&ref);
	}

	PIActionReference release()
	{
		this->owns = false;
		return (ref); 
	}

private:
	bool owns;*/
};

class Auto_Desc
{
	private:

		PIActionDescriptor descriptor;

		ActionDescriptorSuite const & actionDescriptorSuite;

	public:

		Auto_Desc(ActionDescriptorSuite const & actionDescriptorSuite, bool make = true)
			: descriptor(nullptr), actionDescriptorSuite(actionDescriptorSuite)
		{
			if (make && (nullptr != actionDescriptorSuite))
			{
				actionDescriptorSuite->Make(&descriptor);
			}
		}

		~Auto_Desc()
		{
			if (nullptr != descriptor)
			{
				actionDescriptorSuite->Free(descriptor);
			}
		}

		PIActionDescriptor * operator & ()
		{
			return (&descriptor);
		}

		PIActionDescriptor get() const
		{
			return descriptor; 
		}
};

#endif
