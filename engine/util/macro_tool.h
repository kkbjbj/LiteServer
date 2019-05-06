#ifndef MACRO_TOOL_H
#define MACRO_TOOL_H

#define SAFE_DELETE(x)\
do\
{\
	if(x){delete (x);(x)=0;}\
}while(0);

#define SAFE_RELEASE(x)\
do\
{\
	if(x){(x)->Release();(x)=0;}\
}while(0);

#define SAFE_RESTORE(x)\
do\
{\
	if(x){(x)->Restore();(x)=0;}\
}while(0);

#endif
