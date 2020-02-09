#include <mw/core/traits/Serializable.h>
#include <mw/core/serialization/Serializer.h>

namespace Traits
{
    std::vector<uint8_t> ISerializable::Serialize() const
    {
        Serializer serializer;
        Serialize(serializer);
        return serializer.vec();
    }
}