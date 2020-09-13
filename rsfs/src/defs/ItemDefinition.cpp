#include <rsfs/defs/ItemDefinition.hpp>
#include <rsfs/jag/Index.hpp>

#include <glog/logging.h>

using namespace rsfs;

/**
 * The buffer to read from.
 * @param fs    The filesystem to read from.
 * @param id    The item id.
 * @return      The item definition.
 */
ItemDefinition ItemDefinition::forId(RSFileSystem& fs, size_t id)
{
    auto& items = fs.getIndex(Index::CONFIG_OBJ);
    auto data   = items.data(id >> 8u, id & 0xFFu);
    return decode(data);
}

/**
 * Decodes an item definition from a buffer.
 * @param buf   The buffer to read from.
 * @return      The item definition.
 */
ItemDefinition ItemDefinition::decode(RSBuffer& buf)
{
    ItemDefinition def;
    uint8_t opcode;
    while ((opcode = buf.readByte()) != 0)
    {
        if (opcode == 1)
            def.model_ = buf.readShort();
        if (opcode == 2)
            def.name_ = buf.readString();
        if (opcode == 4)
            def.spriteScale_ = buf.readShort();
        if (opcode == 5)
            def.spritePitch_ = buf.readShort();
        if (opcode == 6)
            def.spriteCameraRoll_ = buf.readShort();
        if (opcode == 7)
            def.spriteTranslateX_ = buf.readShort();
        if (opcode == 8)
            def.spriteTranslateY_ = buf.readShort();
        if (opcode == 11)
            def.stackable_ = true;
        if (opcode == 12)
            def.value_ = buf.readInt();
        if (opcode == 16)
            def.members_ = true;
        if (opcode == 18)
            def.stackSize_ = buf.readShort();
        if (opcode == 23)
            def.primaryMaleModel_ = buf.readShort();
        if (opcode == 24)
            def.secondaryMaleModel_ = buf.readShort();
        if (opcode == 25)
            def.primaryFemaleModel_ = buf.readShort();
        if (opcode == 26)
            def.secondaryFemaleModel_ = buf.readShort();
        if (opcode >= 30 && opcode < 35)
            def.groundOptions_[opcode - 30] = buf.readString();
        if (opcode >= 35 && opcode < 40)
            def.options_[opcode - 35] = buf.readString();
        if (opcode == 40)
        {
            auto size     = buf.readByte();
            auto& colours = def.colourModifications_;
            colours.resize(size);

            for (auto i = 0; i < size; i++)
            {
                colours.at(i) = std::pair(buf.readShort(), buf.readShort());
            }
        }
        if (opcode == 41)
        {
            auto size      = buf.readByte();
            auto& textures = def.textureModifications_;
            textures.resize(size);

            for (auto i = 0; i < size; i++)
            {
                textures.at(i) = std::pair(buf.readShort(), buf.readShort());
            }
        }
    }
    return def;
}