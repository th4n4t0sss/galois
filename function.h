#include <math.h>

/* testing prime numbers */
bool is_prime(int n) {
	if (n <= 1)
		return false;
	for (int i=2;i<=sqrt(n);++i) {
		if (n % i == 0)
            return false;
	}
	return true;
}

int primes(int x) {
	if (is_prime(x))
		return x;
	return primes(x + 1);
}
/* TODO: text prompt for inserting mathematical function */
double f(double x) {
	return x;
}
