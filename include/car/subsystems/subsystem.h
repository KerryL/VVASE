
#include "utilities/componentManager.h"

namespace VVASE
{

class Subsystem : public RegisterableComponent
{
public:
	virtual wxPanel* GetEditPanel() = 0;
	virtual wxTreeListItem* GetTreeItem() = 0;
	
	// File read/write functions
	virtual void Write(BinaryWriter& file) const = 0;
	virtual void Read(BinaryReader& file, const int& fileVersion) = 0;
};

}// namespace VVASE
