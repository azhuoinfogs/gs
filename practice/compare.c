

int my_strcmp( char *a, char *b )                //比较字符串每个字符的大小
{
	while((*a != '\0') && (*b != '\0')) {
		if ( *a > *b) {
        	    return 0;
	        } else if ( *a < *b ) {
        	    return 1;
		} else {
			a++;
			b++;
		}
    	}
	if((*a == '\0') && (*b != '\0')) {
		return 1;
	} else if((*a != '\0') && (*b == '\0')){
	        return 0;
    	} else {
        	return 2;
    	}
}

