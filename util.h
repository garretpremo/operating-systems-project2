#ifndef UTIL_H_
#define UTIL_H_

int max(int a, int b) {
    if( a < b ) {return b;}
    else { return a; }
}

int min(int a, int b) {
	if( b < a) {return b;}
	else {return a; }
}

#endif // UTIL_H_