#pragma once

#include <rsfs/RSFileSystem.hpp>
#include <rsfs/io/RSBuffer.hpp>

#include <vector>

/**
 * The number of options for the inventory/ground actions.
 */
constexpr const auto NUM_OPTIONS = 5;

namespace rsfs
{
    /**
     * Represents the definition of an index.
     */
    class ItemDefinition
    {
    public:
        /**
         * Gets the item definition for an item with a specified id.
         * @param fs    The filesystem to read from.
         * @param id    The item id.
         * @return      The item definition.
         */
        static ItemDefinition forId(RSFileSystem& fs, size_t id);

        /**
         * Decodes an item definition from a buffer.
         * @param buf   The buffer to read from.
         * @return      The item. definition.
         */
        static ItemDefinition decode(RSBuffer& buf);

        /**
         * Gets the name of this item.
         * @return  The name.
         */
        [[nodiscard]] std::string_view name() const
        {
            return name_;
        }

    private:
        /**
         * The model id of this item.
         */
        size_t model_{ 0 };

        /**
         * The name of this item.
         */
        std::string name_{ "" };

        /**
         * If this item is members-only.
         */
        bool members_{ false };

        /**
         * The right-click inventory options for this item.
         */
        std::array<std::string, NUM_OPTIONS> options_{ "" };

        /**
         * The right-click ground options for this item.
         */
        std::array<std::string, NUM_OPTIONS> groundOptions_{ "" };

        /**
         * The shop value of this item.
         */
        size_t value_{ 0 };

        /**
         * If the item is stackable
         */
        bool stackable_{ false };

        /**
         * If the item is stackable, this indicates the stack size of the item.
         */
        size_t stackSize_{ 0 };

        /**
         * The primary male model id.
         */
        size_t primaryMaleModel_{ 0 };

        /**
         * The secondary male model id.
         */
        size_t secondaryMaleModel_{ 0 };

        /**
         * The primary female model id.
         */
        size_t primaryFemaleModel_{ 0 };

        /**
         * The secondary female model id.
         */
        size_t secondaryFemaleModel_{ 0 };

        /**
         * The scale of the inventory sprite.
         */
        size_t spriteScale_{ 0 };

        /**
         * The rotation around the x-axis
         */
        size_t spritePitch_{ 0 };

        /**
         * The rotation around the y-axis
         */
        size_t spriteCameraRoll_{ 0 };

        /**
         * The x translation of the inventory sprite.
         */
        size_t spriteTranslateX_{ 0 };

        /**
         * The y translation of the inventory sprite.
         */
        size_t spriteTranslateY_{ 0 };

        /**
         * A vector of colour modification pairs in the format of (old, new)
         */
        std::vector<std::pair<size_t, size_t>> colourModifications_{};

        /**
         * A vector of texture modification pairs in the format of (old, new)
         */
        std::vector<std::pair<size_t, size_t>> textureModifications_{};
    };
}