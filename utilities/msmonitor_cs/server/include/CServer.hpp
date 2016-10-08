#include <CComS.hpp>
#include <CMeasure.hpp>

class CServer{

public:
	CServer();
	~CServer();
	int init();
	
private:
	CMeasure* mMeasure;
	CComS* mCom;
	
};