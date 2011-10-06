#pragma once

void	wave2str		(int wave, char* text);
void	myfloat2string	(float val, char* text);
void	bool2string		(bool val, char* text);

int		c_val2fine		(float value);
float	c_fine2val		(int value);
int		c_val2coarse	(float value);
float	c_coarse2val	(int value);
float	c_val2cutoff	(float value);
float	c_cutoff2val	(float value);
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
