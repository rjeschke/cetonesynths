#pragma once

void	wave2str		(int wave, char* text);
void	owave2str		(int wave, char* text);
void	fmode2str		(int mode, char* text);
void	ftype2str		(int mode, char* text);
void	msrc2str		(int val, char* text);
void	mdest2str		(int val, char* text);
void	arp2str			(int val, char* text);
void	myfloat2string	(float val, char* text);
void	bool2string		(bool val, char* text);

int		c_val2fine		(float value);
float	c_fine2val		(int value);
int		c_val2coarse	(float value);
float	c_coarse2val	(int value);
float	c_val2cutoff	(float value);
float	c_cutoff2val	(float value);
int		c_val2pw		(float value);
float	c_pw2val		(int value);
bool	c_val2bool		(float value);
float	c_bool2val		(bool value);

int		pf2i			(float val, int max);
float	pi2f			(int val, int max);

inline int truncate(float flt)
{
    int i;
    _asm
    {
        fld flt
        fistp i
    }
    return i;
}