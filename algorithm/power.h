bool gInvalidInput = false;
double PowerWithUnsignedExponent(double base, unsigned int exponent);
double PowerWithUnsignedExponent2(double base, unsigned int exponent);
double PowerWithUnsignedExponent3(double base, unsigned int exponent);
bool equal(double val1, double val2)
{
	if ((val1 - val2 >= -0.0000001) &&
		(val1 - val2 <= 0.0000001))
	{
		return true;
	}
	else
		return false;
}

double power(double base, int exponent)
{
	gInvalidInput = false;
	if (equal(base, 0.0) && exponent < 0)
	{
		gInvalidInput = true;
		return 0.0;
	}
	unsigned int absExponent = (unsigned int)exponent;
	if (exponent < 0)
		absExponent = (unsigned int)-exponent;
	double result = PowerWithUnsignedExponent(base, absExponent);
	if (exponent < 0)
	{
		result = 1.0 / result;
	}
	return result;
}

double PowerWithUnsignedExponent(double base, unsigned int exponent)
{
	double result = 1.0;
	for (unsigned int i = 0; i < exponent; ++i)
	{
		result *= base;
	}
	return result;
}

double power2(double base, int exponent)
{
	gInvalidInput = false;
	if (equal(base, 0.0) && exponent < 0)
	{
		gInvalidInput = true;
		return 0.0;
	}
	unsigned int absExponent = (unsigned int)exponent;
	if (exponent < 0)
		absExponent = (unsigned int)-exponent;
	double result = PowerWithUnsignedExponent2(base, absExponent);
	if (exponent < 0)
	{
		result = 1.0 / result;
	}
	return result;
}

double PowerWithUnsignedExponent2(double base, unsigned int exponent)
{
	double result = 1.0;
	while (exponent)
	{
		if ((exponent & 0x1) == 1)
		{
			result *= base;
		}
		base *= base;
		exponent = exponent >> 1;
	}
	return result;
}
//递归实现
double power3(double base, int exponent)
{
	gInvalidInput = false;
	if (equal(base, 0.0) && exponent < 0)
	{
		gInvalidInput = true;
		return 0.0;
	}
	unsigned int absExponent = (unsigned int)exponent;
	if (exponent < 0)
		absExponent = (unsigned int)-exponent;
	double result = PowerWithUnsignedExponent3(base, absExponent);
	if (exponent < 0)
	{
		result = 1.0 / result;
	}
	return result;
}

double PowerWithUnsignedExponent3(double base, unsigned int exponent)
{
	if (exponent == 0)
	{
		return 1;
	}
	else if (exponent == 1)
	{
		return base;
	}

	if ((exponent & 0x1) == 1)
	{
		return base * PowerWithUnsignedExponent3(base * base, exponent >> 1);
	}
	else
	{
		return PowerWithUnsignedExponent3(base * base, exponent >> 1);
	}
}