#pragma once

enum AttributeSpecifiers
{
	//! @brief Marks the class as abstract.
	Abstract,

	//! @brief Set the user-friendly name for the editor.
	DisplayName,

	//! @brief Set the category of the field.
	Category,

	//! @brief Set the order of the category.
	CategoryOrder,

	//! @brief Indicates that the field is editable anywhere.
	EditAnywhere,

	//! @brief This makes the field visible, but not editable.
	VisibleAnywhere,

	//! @brief Indicates that the field is read only.
	ReadOnly,

	//! @brief This will exclude the field from serialization.
	NonSerialized
};

/*
 * Used to mark a class as reflected.
 * Attribute Usage:
 *  Abstract // Must be set if the class is either abstract or doesn't have default constructor
 *  DisplayName = "Struct Display Name" // Set the display name of the class
 */
#define CLASS(...)

/*
 * Used to mark a struct as reflected.
 * Attribute Usage:
 *  DisplayName = "Struct Display Name" // Set the display name of the struct
 */
#define STRUCT(...)


/*
 * Used to mark an enum as reflected.
 * Attribute Usage:
 *  DisplayName = "Enum Display Name" // Set the display name of the enum
 */
#define ENUM(...)

/*
 * Used to set attributes for a constant inside a reflected enum
 * Attribute Usage:
 *  DisplayName = "Enum Constant Display Name" // Set the display name of the enum constant
 */
#define ECONST(...)

/*
 * Used to mark a field as reflected.
 */
#define FIELD(...)

/*
 * Used to mark a function as reflected.
 * Attribute Usage:
 *  Event // Mark the function as an event, to be bound to a signal or an event bus
 */
#define FUNCTION(...)

#define EVENT(...)
