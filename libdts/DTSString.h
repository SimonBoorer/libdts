#ifndef DTS_STRING_H_
#define DTS_STRING_H_

#include <string>
#include <cstdint>

namespace DTS
{

namespace String
{

std::string GetTrailingNumber(const char* str, int32_t& number)
{
	// Check for trivial strings
	if (!str || !str[0])
		return "";

	// Find the number at the end of the string
	std::string base(str);
	const char* p = base.c_str() + base.length() - 1;

	// Ignore trailing whitespace
	while ((p != base.c_str()) && isspace(*p))
		p--;

	// Need at least one digit!
	if (!isdigit(*p))
		return base;

	// Back up to the first non-digit character
	while ((p != base.c_str()) && isdigit(*p))
		p--;

	// Convert number => allow negative numbers, treat '_' as '-' for Maya
	if ((*p == '-') || (*p == '_'))
		number = -std::atoi(p + 1);
	else
		number = ((p == base.c_str()) ? std::atoi(p) : std::atoi(++p));

	// Remove space between the name and the number
	while ((p > base.c_str()) && isspace(*(p - 1)))
		p--;

	return base.substr(0, p - base.c_str());
}

// Check if one string starts with another
bool StartsWith(const std::string& str1, const std::string& str2)
{
	return str2.length() <= str1.length()
		&& std::equal(str2.begin(), str2.end(), str1.begin());
}

} // namespace String

} // namespace DTS

#endif // DTS_STRING_H_
