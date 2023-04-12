#pragma once

/*
 * Used to mark a class as reflected.
 * Attribute Usage:
 *  Abstract // Must be set if the class is either abstract or doesn't have default constructor
 *  Display = "Struct Display Name" // Set the display name of the class
 */
#define CLASS(...)

/*
 * Used to mark a struct as reflected.
 * Attribute Usage:
 *  Display = "Struct Display Name" // Set the display name of the struct
 */
#define STRUCT(...)


/*
 * Used to mark an enum as reflected.
 * Attribute Usage:
 *  Display = "Enum Display Name" // Set the display name of the enum
 */
#define ENUM(...)

/*
 * Used to set attributes for a constant inside a reflected enum
 * Attribute Usage:
 *  Display = "Enum Constant Display Name" // Set the display name of the enum constant
 */
#define ECONST(...)


/*
 * Used to mark a field as reflected.
 * Attribute Usage:
 *  Hidden // Marks the field as hidden
 *  Display = "My Field" // Set the display name of the field
 */
#define FIELD(...)

/*
 * Used to mark a function as reflected.
 * Attribute Usage:
 *  Event // Mark the function as an event, to be bound to a signal or an event bus
 */
#define FUNCTION(...)

#define EVENT(...)
