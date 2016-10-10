#include <CComS.hpp>
#include <CMeasure.hpp>

class CServer{

public:
	CServer();
	~CServer();
	int init();
	
private:
	int mSockfd;
	CMeasure* mMeasure;
	CComS* mCom;
	
};