#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#undef strlen

#ifndef STRLEN
# define STRLEN strlen
#endif

int
memcmp (const void *str1, const void *str2, size_t count)
{
  const unsigned char *s1 = str1;
  const unsigned char *s2 = str2;

  while (count-- > 0)
    {
      if (*s1++ != *s2++)
	  return s1[-1] < s2[-1] ? -1 : 1;
    }
  return 0;
}

void *
memcpy (void *dest, const void *src, size_t len)
{
  char *d = dest;
  const char *s = src;
  while (len--)
    *d++ = *s++;
  return dest;
}

void *
memset (void *dest, register int val, register size_t len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}

/* Return the length of the null-terminated string STR.  Scan for
   the null terminator quickly by testing four bytes at a time.  */
size_t
strlen (const char *str)
{
  const char *char_ptr;
  const unsigned long int *longword_ptr;
  unsigned long int longword, himagic, lomagic;

  /* Handle the first few characters by reading one character at a time.
     Do this until CHAR_PTR is aligned on a longword boundary.  */
  for (char_ptr = str; ((unsigned long int) char_ptr
			& (sizeof (longword) - 1)) != 0;
       ++char_ptr)
    if (*char_ptr == '\0')
      return char_ptr - str;

  /* All these elucidatory comments refer to 4-byte longwords,
     but the theory applies equally well to 8-byte longwords.  */

  longword_ptr = (unsigned long int *) char_ptr;

  /* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
     the "holes."  Note that there is a hole just to the left of
     each byte, with an extra at the end:

     bits:  01111110 11111110 11111110 11111111
     bytes: AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD

     The 1-bits make sure that carries propagate to the next 0-bit.
     The 0-bits provide holes for carries to fall into.  */
  himagic = 0x80808080L;
  lomagic = 0x01010101L;
  if (sizeof (longword) > 4)
    {
      /* 64-bit version of the magic.  */
      /* Do the shift in two steps to avoid a warning if long has 32 bits.  */
      himagic = ((himagic << 16) << 16) | himagic;
      lomagic = ((lomagic << 16) << 16) | lomagic;
    }
  if (sizeof (longword) > 8)
    abort ();

  /* Instead of the traditional loop which tests each character,
     we will test a longword at a time.  The tricky part is testing
     if *any of the four* bytes in the longword in question are zero.  */
  for (;;)
    {
      longword = *longword_ptr++;

      if (((longword - lomagic) & ~longword & himagic) != 0)
	{
	  /* Which of the bytes was the zero?  If none of them were, it was
	     a misfire; continue the search.  */

	  const char *cp = (const char *) (longword_ptr - 1);

	  if (cp[0] == 0)
	    return cp - str;
	  if (cp[1] == 0)
	    return cp - str + 1;
	  if (cp[2] == 0)
	    return cp - str + 2;
	  if (cp[3] == 0)
	    return cp - str + 3;
	  if (sizeof (longword) > 4)
	    {
	      if (cp[4] == 0)
		return cp - str + 4;
	      if (cp[5] == 0)
		return cp - str + 5;
	      if (cp[6] == 0)
		return cp - str + 6;
	      if (cp[7] == 0)
		return cp - str + 7;
	    }
	}
    }
}

/* struct qr - stores qutient/remainder to handle divmod EABI interfaces. */
struct qr {
	unsigned q;		/* computed quotient */
	unsigned r;		/* computed remainder */
	unsigned q_n;		/* specficies if quotient shall be negative */
	unsigned r_n;		/* specficies if remainder shall be negative */
};

static void uint_div_qr(unsigned numerator, unsigned denominator,
			struct qr *qr);

/* returns in R0 and R1 by tail calling an asm function */
unsigned __aeabi_uidivmod(unsigned numerator, unsigned denominator);

unsigned __aeabi_uidiv(unsigned numerator, unsigned denominator);

/* returns in R0 and R1 by tail calling an asm function */
signed __aeabi_idivmod(signed numerator, signed denominator);

signed __aeabi_idiv(signed numerator, signed denominator);

/*
 * __ste_idivmod_ret_t __aeabi_idivmod(signed numerator, signed denominator)
 * Numerator and Denominator are received in R0 and R1.
 * Where __ste_idivmod_ret_t is returned in R0 and R1.
 *
 * __ste_uidivmod_ret_t __aeabi_uidivmod(unsigned numerator,
 *                                       unsigned denominator)
 * Numerator and Denominator are received in R0 and R1.
 * Where __ste_uidivmod_ret_t is returned in R0 and R1.
 */
// should be fine with no return
// see https://github.com/OP-TEE/optee_os/blob/master/lib/libutils/isoc/arch/arm/arm32_aeabi_divmod_a32.S
//signed ret_idivmod_values(signed quotient, signed remainder);
//unsigned ret_uidivmod_values(unsigned quotient, unsigned remainder);

static void division_qr(unsigned n, unsigned p, struct qr *qr)
{
	unsigned i = 1, q = 0;
	if (p == 0) {
		qr->r = 0xFFFFFFFF;	/* division by 0 */
		return;
	}

	while ((p >> 31) == 0) {
		i = i << 1;	/* count the max division steps */
		p = p << 1;     /* increase p until it has maximum size*/
	}

	while (i > 0) {
		q = q << 1;	/* write bit in q at index (size-1) */
		if (n >= p)
		{
			n -= p;
			q++;
		}
		p = p >> 1; 	/* decrease p */
		i = i >> 1; 	/* decrease remaining size in q */
	}
	qr->r = n;
	qr->q = q;
}

static void uint_div_qr(unsigned numerator, unsigned denominator, struct qr *qr)
{

	division_qr(numerator, denominator, qr);

	/* negate quotient and/or remainder according to requester */
	if (qr->q_n)
		qr->q = -qr->q;
	if (qr->r_n)
		qr->r = -qr->r;
}

unsigned __aeabi_uidiv(unsigned numerator, unsigned denominator)
{
	struct qr qr = { .q_n = 0, .r_n = 0 };

	uint_div_qr(numerator, denominator, &qr);

	return qr.q;
}

unsigned  __aeabi_uidivmod(unsigned numerator, unsigned denominator)
{
	struct qr qr = { .q_n = 0, .r_n = 0 };

	uint_div_qr(numerator, denominator, &qr);

	//return ret_uidivmod_values(qr.q, qr.r);
	return;
}

signed __aeabi_idiv(signed numerator, signed denominator)
{
	struct qr qr = { .q_n = 0, .r_n = 0 };

	if (((numerator < 0) && (denominator > 0)) ||
	    ((numerator > 0) && (denominator < 0)))
		qr.q_n = 1;	/* quotient shall be negate */
	if (numerator < 0) {
		numerator = -numerator;
		qr.r_n = 1;	/* remainder shall be negate */
	}
	if (denominator < 0)
		denominator = -denominator;

	uint_div_qr(numerator, denominator, &qr);

	return qr.q;
}

signed  __aeabi_idivmod(signed numerator, signed denominator)
{
	struct qr qr = { .q_n = 0, .r_n = 0 };

	if (((numerator < 0) && (denominator > 0)) ||
	    ((numerator > 0) && (denominator < 0)))
		qr.q_n = 1;	/* quotient shall be negate */
	if (numerator < 0) {
		numerator = -numerator;
		qr.r_n = 1;	/* remainder shall be negate */
	}
	if (denominator < 0)
		denominator = -denominator;

	uint_div_qr(numerator, denominator, &qr);

	//return ret_idivmod_values(qr.q, qr.r);
	return;
}
