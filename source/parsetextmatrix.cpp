#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>


static int maxExponent = 511;	/* Largest possible base 10 exponent.  Any
                 * exponent larger than this will already
                 * produce underflow or overflow, so there's
                 * no need to worry about additional digits.
                 */
static double powersOf10[] = {	/* Table giving binary powers of 10.  Entry */
    10.,			/* is 10^2^i.  Used to convert decimal */
    100.,			/* exponents into floating-point numbers. */
    1.0e4,
    1.0e8,
    1.0e16,
    1.0e32,
    1.0e64,
    1.0e128,
    1.0e256
};

/*
 *----------------------------------------------------------------------
 *
 * strtod --
 *
 *	This procedure converts a floating-point number from an ASCII
 *	decimal representation to internal double-precision format.
 *
 * Results:
 *	The return value is the double-precision floating-point
 *	representation of the characters in string.  If endPtr isn't
 *	NULL, then *endPtr is filled in with the address of the
 *	next character after the last one that was part of the
 *	floating-point number.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

#define TRUE 1
#define FALSE 0

double strtod2(char *string, char **endPtr)
{
    int sign, expSign = FALSE;
    double fraction, dblExp, *d;
    const char *p;
    int c;
    int exp = 0;		/* Exponent read from "EX" field. */
    int fracExp = 0;		/* Exponent that derives from the fractional
                 * part.  Under normal circumstatnces, it is
                 * the negative of the number of digits in F.
                 * However, if I is very long, the last digits
                 * of I get dropped (otherwise a long I with a
                 * large negative exponent could cause an
                 * unnecessary overflow on I alone).  In this
                 * case, fracExp is incremented one for each
                 * dropped digit. */
    int mantSize;		/* Number of digits in mantissa. */
    int decPt;			/* Number of mantissa digits BEFORE decimal
                 * point. */
    const char *pExp;		/* Temporarily holds location of exponent
                 * in string. */

    /*
     * Strip off leading blanks and check for a sign.
     */

    p = string;
    while (isspace(*p)) {
    p += 1;
    }
    if (*p == '-') {
    sign = TRUE;
    p += 1;
    } else {
    if (*p == '+') {
        p += 1;
    }
    sign = FALSE;
    }

    /*
     * Count the number of digits in the mantissa (including the decimal
     * point), and also locate the decimal point.
     */

    decPt = -1;
    for (mantSize = 0; ; mantSize += 1)
    {
    c = *p;
    if (!isdigit(c)) {
        if ((c != '.') || (decPt >= 0)) {
        break;
        }
        decPt = mantSize;
    }
    p += 1;
    }

    /*
     * Now suck up the digits in the mantissa.  Use two integers to
     * collect 9 digits each (this is faster than using floating-point).
     * If the mantissa has more than 18 digits, ignore the extras, since
     * they can't affect the value anyway.
     */

    pExp  = p;
    p -= mantSize;
    if (decPt < 0) {
    decPt = mantSize;
    } else {
    mantSize -= 1;			/* One of the digits was the point. */
    }
    if (mantSize > 18) {
    fracExp = decPt - 18;
    mantSize = 18;
    } else {
    fracExp = decPt - mantSize;
    }
    if (mantSize == 0) {
    fraction = 0.0;
    p = string;
    goto done;
    } else {
    int frac1, frac2;
    frac1 = 0;
    for ( ; mantSize > 9; mantSize -= 1)
    {
        c = *p;
        p += 1;
        if (c == '.') {
        c = *p;
        p += 1;
        }
        frac1 = 10*frac1 + (c - '0');
    }
    frac2 = 0;
    for (; mantSize > 0; mantSize -= 1)
    {
        c = *p;
        p += 1;
        if (c == '.') {
        c = *p;
        p += 1;
        }
        frac2 = 10*frac2 + (c - '0');
    }
    fraction = (1.0e9 * frac1) + frac2;
    }

    /*
     * Skim off the exponent.
     */

    p = pExp;
    if ((*p == 'E') || (*p == 'e')) {
    p += 1;
    if (*p == '-') {
        expSign = TRUE;
        p += 1;
    } else {
        if (*p == '+') {
        p += 1;
        }
        expSign = FALSE;
    }
    if (!isdigit(*p)) {
        p = pExp;
        goto done;
    }
    while (isdigit(*p)) {
        exp = exp * 10 + (*p - '0');
        p += 1;
    }
    }
    if (expSign) {
    exp = fracExp - exp;
    } else {
    exp = fracExp + exp;
    }

    /*
     * Generate a floating-point number that represents the exponent.
     * Do this by processing the exponent one bit at a time to combine
     * many powers of 2 of 10. Then combine the exponent with the
     * fraction.
     */

    if (exp < 0) {
    expSign = TRUE;
    exp = -exp;
    } else {
    expSign = FALSE;
    }
    if (exp > maxExponent) {
    exp = maxExponent;
    //errno = ERANGE;
    }
    dblExp = 1.0;
    for (d = powersOf10; exp != 0; exp >>= 1, d += 1) {
    if (exp & 01) {
        dblExp *= *d;
    }
    }
    if (expSign) {
    fraction /= dblExp;
    } else {
    fraction *= dblExp;
    }

done:
    if (endPtr != NULL) {
    *endPtr = (char *) p;
    }

    if (sign) {
    return -fraction;
    }
    return fraction;
}


struct fastarray {
    void *buffer;
    unsigned buffersize;
    unsigned bufferused;
};
void fastarrayinit(fastarray &a)
{
    a.buffer=0;
    a.buffersize=0;
    a.bufferused=0;
}
void fastarrayfree(fastarray &a)
{
    if(a.buffer)
        free(a.buffer);
    fastarrayinit(a);
}
/*
    Arrayadd appends an element to buffer.
    buffersize is the current buffersize
    bufferused is how many entries are used
    elemsz is the byte size of the elements in the buffer

    Note: buffer, buffersize, bufferused are modified by this call
*/
bool fastarrayadd(fastarray &a,unsigned elemsz,void *elem)
{
    if(a.buffersize<=a.bufferused)
    {
        a.buffersize*=2;
        if(a.buffersize==0)
            a.buffersize=1;
        //printf("Realloc %d\n",a.buffersize);
        a.buffer = realloc(a.buffer,a.buffersize*elemsz);
        if(!a.buffer)
            return false;
    }
    memcpy((char*)a.buffer + a.bufferused*elemsz,elem,elemsz);
    a.bufferused++;
    return true;
}


/*
    0: ok
    offset | 0x01: parse error
    offset | 0x02: memory error
*/
unsigned parsetextmatrix(char *data,int *sx,int *sy,double **outdata,bool **outnan)
{
    unsigned idx=0;
    unsigned idx2;
    char t;
    double v;
    bool nan;
    bool gotlinedata;
    bool first=true;
    char *endptr;
    unsigned col=0,numcol=0;
    int errcode=0;
    fastarray bufferval,buffernan;

    fastarrayinit(bufferval);
    fastarrayinit(buffernan);
    gotlinedata=false;		// Indicate ANY data on the line (i.e. non empty line, even if only nan or commas)
    bool expectdata=true;

    // Indicate no memory allocated so far
    *outdata = 0;
    *outnan = 0;
    // Indicate no matrix so far
    *sx = 0;
    *sy = 0;

    idx--;

    while(1)    // byte at data[len] is 0, guaranteed by the callee.
    {
        idx++;

        // Skip any space
        if(data[idx]==32 || data[idx]=='\t')
            continue;

        // Comma
        if(data[idx]==',')
        {
            // If we expected data, but didn't get, then add a nan
            if(expectdata)
            {
                v = 0.0;
                nan=true;
                if(!fastarrayadd(bufferval,sizeof(double),&v))
                {
                    errcode=2;
                    goto enderror;
                }
                if(!fastarrayadd(buffernan,sizeof(bool),&nan))
                {
                    errcode=2;
                    goto enderror;
                }
                // Number of values in the current column
                col++;
                // got any data on the current line
                gotlinedata=true;
            }
            // Flag that we expect a number next (e.g. if next we get a cr or null, we must insert a nan)
            expectdata=true;
        }
        // Start of a number, the first character i either a digit, a minus sign, a dot, or a nan
        else if((data[idx]>='0' && data[idx]<='9') || data[idx]=='-'  || data[idx]=='.' || data[idx]=='N' || data[idx]=='n')
        {
            // Look for next space, tab, or comma. idx2 points to the character after the number
            idx2=idx;
            while(data[idx2]!=0 && data[idx2]!=' ' && data[idx2]!='\t' && data[idx2]!=',' && data[idx2]!=13 && data[idx2]!=10)
                idx2++;

            //printf("token [%d-%d[ (%d %d)\n",idx,idx2,(unsigned)data[idx],(unsigned)data[idx2]);

            t = data[idx2];    		// Keep byte
            data[idx2]=0;           // Null terminate buffer to parse in place


            // Check if the value is a NaN
            if(idx2-idx==3 && (data[idx+0]=='N' || data[idx+0]=='n') && (data[idx+1]=='A' || data[idx+1]=='a') && (data[idx+2]=='N' || data[idx+2]=='n'))
            {
                // number is a nan
                nan=true;
                v = 0.0;
            }
            else
            {
                // Conver number
                v = strtod2(data+idx,&endptr);
                //printf("strtod: %d\n",v);


                // The endptr points to character idx2 if the conversion is successful
                if(endptr!=data+idx2)
                {
                    //printf("error x\n");
                    errcode=1;
                    goto enderror;
                }
                nan=false;
            }
            data[idx2]=t;           // Restore buffer to initial state

            if(!fastarrayadd(bufferval,sizeof(double),&v))
            {
                errcode=2;
                goto enderror;
            }
            if(!fastarrayadd(buffernan,sizeof(bool),&nan))
            {
                errcode=2;
                goto enderror;
            }

            // Number of values in the current column
            col++;
            // got any data on the current line
            gotlinedata=true;
            // don't expect data anymore
            expectdata=false;

            idx=idx2-1;

            /*if(nan)
            printf("NaN ");
            else
            printf("%lf ",v);*/
            continue;
        }
        // Newline or end of file
        else if(data[idx]==13 || data[idx]==10 || data[idx]==0)
        {
            if(expectdata && col!=0)	// only add data if we expect, but also have at least read a val (empty newlines expect value)s
            {
                // We are expecting data after a nan -> add a nan
                v = 0.0;
                nan=true;
                if(!fastarrayadd(bufferval,sizeof(double),&v))
                {
                    errcode=2;
                    goto enderror;
                }
                if(!fastarrayadd(buffernan,sizeof(bool),&nan))
                {
                    errcode=2;
                    goto enderror;
                }
                // Number of values in the current column
                col++;
                // got any data on the current line
                gotlinedata=true;
            }
            // Got a newline. Determine column width and/or column width mismatch
            if(gotlinedata==true)
            {
                //printf("\n");

                if(first)
                {
                   numcol = col;
                   //printf("Determined columns is %d\n",numcol);
                   first=false;
                }
                else
                {
                    //printf("not first, checking num col %d %d\n",numcol, col);
                    if(numcol != col)
                    {
                        errcode=1;
                        goto enderror;
                    }
                }

            }
            // Set state for the next line
            col = 0;
            gotlinedata=false;
            // On the next line we will expect data
            expectdata=true;
        }
        // Anythine else than the start of a number or newline or end of file is an error
        else
        {
            //printf("error y\n");
            errcode=1;
            goto enderror;
        }

        // Check end of buffer
        if(data[idx]==0)
            break;
    }	// while(1);

    goto endok;


    enderror:
    //printf("enderror, errcode: %d. idx: %d\n",errcode,idx);
    //if(errcode==1)
        //printf("Parse error at offset %d\n",idx);
    fastarrayfree(bufferval);
    fastarrayfree(buffernan);
    return errcode | (idx<<8);

endok:
    //printf("Read %d values\n",bufferval.bufferused);
    *sx = numcol;
    if(numcol!=0)
        *sy = bufferval.bufferused/(*sx);
    *outdata = (double*)bufferval.buffer;
    *outnan = (bool*)buffernan.buffer;
    //printf("%d x %d \n",*sx,*sy);
    return 0;
}
