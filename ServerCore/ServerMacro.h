/*--------------------
		CRASH
---------------------*/

#define CRASH(cause)						\
{											\
	unsigned __int32* crash = nullptr;		\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}		

#define ASSERT_CRASH(expr)					\
{									\
	if(!(expr))						\
	{								\
		CRASH("ASSERT CRASH")		\
		__analysis_assume(expr);	\
	}								\
}				

#define PRINT_ERROR(msg) wcout << "[" << __FILE__ << "]: " << __func__ << "(" << __LINE__ << ") " << L#msg << endl
#define PRINT_WSA_ERROR(msg) wcout << L#msg << ": " << WSAGetLastError() << "(errorCode)" << endl
