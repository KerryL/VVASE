
#include <string>

namespace VVASE
{

class VVASEPlugin
{
public:
	bool Activate() = 0;
	void Deactivate() = 0;

	// TODO:  Would be nice if these were static...
	std::string GetVersionString() const = 0;
};

}// namespace VVASE