# Bundle File Format Specification

The `Bundle` class is used by the engine to store & load assets & serialized objects. It stores the data in a binary file with `.casset` extension.

This document describes the file format specification of `.casset` files. The current format version is 3.0.

# Format Specification


### Basic Types

| Type | Value | Description |
|---|---|---|
| String | `T e x t 00` | Null terminated string |

## **File Structure**
| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 8B | `00 42 55 4e 44 4c 45 00` | Magic Number: `. B U N D L E .` |
| +08 | 4B | `00 00 00 00` | Header checksum |
| +0C | 4B | 3 | Major Version (u32) |
| +10 | 4B | 0 | Minor Version (u32) |
| +14 | 4B | 0 | Patch Version (u32) |
| +1C | 8B | `00 00 00 00 00 00 00 00` | [Schema Table](#schema-table) start offset (from start of file) |
| +24 | 8B | `00 00 00 00 00 00 00 00` | [Serialized Data](#serialized-data) start offset (from start of file) |
|  | | **<-- Header fields start below -->** |  |
| +2C | 16B | 128 bit Uuid | Bundle UUID |
| +3C | \0 | `/Game/Assets/MyTextureAsset\0` | Full path/name of bundle (String) |
| +xx | 4B | `01 00 00 00` | No. of external bundle dependencies |
| - | - | *bundle dependency list (repeat Uuid)* | - |
| +xx | 16B | 128 bit Uuid | n-th Bundle dependency (Uuid). |
| +10 | 1B | `00/01` | Is cooked? |
| - | - | *newly added header fields go here* | - |
| - | - | *schema table goes here* | - |
| - | - | *serialized data goes here* | - |


## **Schema Table**

Schema table stores the layout and offsets of each object and struct that is serialized in this bundle.

| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 8B | `xx xx xx xx xx xx xx xx` | Size of this table in bytes (excluding this field but including End of schema table) |
| +08 | 4B | N | Number of entries in schema |
| | | | |
| | | | |
|  |  | **<-- Entry #0 -->** |  |
| +0C | 4B | `xx xx xx xx` | Size of 1-st schema entry in bytes. (including this field) |
| +10 | 1B | `00/01` | 0 = Class ; 1 = Struct |
| +11 | \0 | `/Code/Core.CE::SomeClass\0` | Full type name of the class or struct |
| +xx | 4B | `00 00 00 00` | Class/Struct Version (Unused) |
| +04 | 4B | n1 | Number of fields |
|  |  | Field #0 |  |
| +08 | \0 | `fieldName0` | Name of the field as in C++ |
| +xx | 1B | `xx` | [Field Type](#field-type) byte. Indicates the field's data type. |
| +xx | 4B | 0-based index into [Schema Table](#schema-table) | (**Optional_1**) Present only if [Field Type](#field-type) byte requires.  |
| +xx | 1B | `xx` Underlying [Field Type](#field-type) byte | (**Optional_2**) Present only if [Field Type](#field-type) byte requires. |
|  |  | Field #1 |  |
| +xx | \0 | `fieldName1` | Name of the field as in C++ |
| +xx | 1B | `xx` | [Field Type](#field-type) byte. Indicates the field's data type. |
| +xx | 4B | 0-based index into [Schema Table](#schema-table) | (**Optional_1**) Present only if [Field Type](#field-type) byte requires.  |
| +xx | 1B | `xx` Underlying [Field Type](#field-type) byte | (**Optional_2**) Present only if [Field Type](#field-type) byte requires. |
| | | | |
| | | | |
|  |  | **<-- Entry #1 -->** |  |
| +xx | 4B | `xx xx xx xx` | Size of 2-nd schema entry in bytes. (including this field) |
| +04 | 1B | `00/01` | 0 = Class ; 1 = Struct |
| +05 | \0 | `/Code/Core.CE::SomeStruct\0` | Full type name of the class or struct |
| +xx | 4B | `00 00 00 00` | Class/Struct Version (Unused) |
| +04 | 4B | n2 | Number of fields |
|  |  | Field #0 |  |
| +08 | \0 | `fieldName0` | Name of the field as in C++ |
| +xx | 1B | `xx` | [Field Type](#field-type) byte. Indicates the field's data type. |
| +xx | 4B | 0-based index into [Schema Table](#schema-table) | (**Optional_1**) Present only if [Field Type](#field-type) byte requires.  |
| +xx | 1B | `xx` Underlying [Field Type](#field-type) byte | (**Optional_2**) Present only if [Field Type](#field-type) byte requires. |
|  |  | Field #1 |  |
| +xx | \0 | `fieldName1` | Name of the field as in C++ |
| +xx | 1B | `xx` | [Field Type](#field-type) byte. Indicates the field's data type. |
| +xx | 4B | 0-based index into [Schema Table](#schema-table) | (**Optional_1**) Present only if [Field Type](#field-type) byte requires.  |
| +xx | 1B | `xx` Underlying [Field Type](#field-type) byte | (**Optional_2**) Present only if [Field Type](#field-type) byte requires. |
| | | | |
| | | | |
|  |  | **<-- Repeating segment ends here -->** |  |
| | 4B | `00 00 00 00` | **End** of schema table |

### Field Type

| Type byte | Type | Description | Requirements in Schema |
|---|---|---|---|
| `00` | null | Null value |
| `01` | u8 | uint8 |
| `02` | u16 | uint16 |
| `03` | u32 | uint32 |
| `04` | u64 | uint64 |
| `05` | s8 | int8 |
| `06` | s16 | int16 |
| `07` | s32 | int32 |
| `08` | s64 | int64 |
| `09` | f32 | float |
| `0A` | f64 | double |
| `0B` | b8 | boolean |
| `0C` | String | String |
| `0D` | Vec2 | Vec2 |
| `0E` | Vec3 | Vec3 |
| `0F` | Vec4 | Vec4 |
| `10` | Array\<Object\> | Array of objects |
| `11` | Array\<Struct\> | Array of struct types | **Optional_1** |
| `12` | Array\<[FieldType](#field-type)\> | Array | **Optional_2** |
| `13` | [Binary](#binary-data-type) | Raw binary data |
| `14` | [Object Ref](#object-reference) | Object reference |
| `15` | [Function Binding](#function-binding) | Function binding to an object (ScriptDelegate). |
| `16` | Array\<[Function Binding](#function-binding)\> | Array of function bindings (ScriptEvent). |
| `17` | [Struct](#struct) | Struct field. | **Optional_1** |

## **Serialized Data**

| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 8B | `xx xx xx xx xx xx xx xx` | Size of this table in bytes (excluding this field but including **End of data** field) |
| +08 | 4B | N | Number of entries in this list. i.e. number of all serialized objects. |
| | | | |
| | | | |
|  |  | **<-- Entry #0 -->** |  |
| +0C | 4B | `xx xx xx xx` | Size of 1-st entry in bytes. (including this field) |
| +10 | 4B | `00 00 00 04` | Data start offset **after** this field. |
| +14 | 16B | 128 bit Uuid | Object Instance UUID. |
| +20 | 1B | `00/01` | Is Asset? `0` or `1` |
| +21 | 4B | Index into [Schema Table](#schema-table) | Index of this class layout in Schema Table. |
| +25 | \0 | `MySkybox.Irradiance.Diffuse\0` | Path to this object within the bundle |
| +xx | \0 | `MyObjectName\0` | Object name (CE::Name) |
| - | - | New header fields can be added here | - |
|  |  | Field #0 | Number of fields can be inferred from Schema Table. |
| +xx | 4B | `00 00 00 04` | Size of 1st field including itself. (>= 4) |
| +04 | xx | [Field Value](#field-value) | [Field Value](#field-value) depending on the [Field Type](#field-type) found in Schema Table. |
|  |  | Field #1 | Number of fields can be inferred from Schema Table. |
| +xx | 4B | `00 00 00 04` | Size of 2nd field including itself. (>= 4) |
| +04 | xx | [Field Value](#field-value) | [Field Value](#field-value) depending on the [Field Type](#field-type) found in Schema Table. |
| | | | |
| | | | |
| | 4B | `00 00 00 00` | **End of data** |

## **Field Value**

### String data type
| Field Type | Size | Format | Desc |
|---|---|---|---|
| String | `\0` | `Null Terminated String\0` | `0` is valid. |

### Binary data type
| Field Type | Size | Format | Desc |
|---|---|---|---|
| Byte Size | 8B | `xx xx xx xx xx xx xx xx` | `0` is valid size. |
| Flags | 8B | `xx xx xx xx xx xx xx xx` | BinaryBlob flags value. |
| xx | xx |  | Raw binary data. |

### Object Reference

| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 16B | 128 bit Uuid | Object UUID. |
| +10 | 16B | 128 bit Uuid | Bundle UUID the referenced object belongs to. |

### NULL Object Reference

| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 16B | `00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00` | 0 Uuid |

### Function Binding

| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 16B | 128 bit Uuid | UUID of the bound **object**. |
| +10 | 16B | 128 bit Uuid | UUID of the **bundle** the bound object is in. |
| +20 | \0 | `FunctionName\0` | Exact name of function as in C++. |

### NULL Function Binding

| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 16B | `00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00` | 0 Uuid. |

### Struct

| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 4B | `00 00 00 04` | Size of 1st field including itself. (>= 4) |
| +04 | xx | [Field Value](#field-value) | Value of 1st field |
| +xx | 4B | `00 00 00 04` | Size of 2nd field including itself. (>= 4) |
| +04 | xx | [Field Value](#field-value) | Value of 2nd field |

### Array

| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 4B | `00 00 00 02` | Number of elements in array  |
| +04 | 4B | `00 00 00 04` | Size of 1st field including itself. (>= 4) |
| +08 | xx | [Field Value](#field-value) | Value of 1st field |
| +xx | 4B | `00 00 00 04` | Size of 2nd field including itself. (>= 4) |
| +04 | xx | [Field Value](#field-value) | Value of 2nd field |



------------
------------
------------
------------

## **Object Entry**
| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 8B | `00 4f 42 4a 45 43 54 00` | Magic Number: `. O B J E C T .` |
| +08 | 8B | `xx xx xx xx xx xx xx xx` | Object Instance UUID |
| +10 | 1B | `01` | Is Asset? `0` or `1` |
| +11 | \0 | `TextureAtlas.Noise.MyNoiseTexture\0` | Virtual path to object within the Bundle. |
| +xx | \0 | `/Engine/Core.CE::Texture\0` | Object class TypeName |
| +10 | \0 | `SomeObjectName\0` | Object name (CE::Name) |
| +xx | \0 | `/Textures/SomeTexture.png` | Source asset path relative to project (exists only if **Is Asset?**) |
| +08 | 8B | `xx xx xx xx xx xx xx xx` | Data start offset (from start of file) |
| +xx | xx | | Newly added header fields |
| +xx | 1B | `10` | [Field Type](#field-type): Map. |
| +xx | xx | | A **[Map](#map)** of fields. |
| +xx | 4B | `xx xx xx xx` | Data CRC checksum. Can be `0`. |


## **Field Value**
| Offset | Size | Value | Description |
|---|---|---|---|
| +xx | 1B | `00` | [Field Type](#field-type) |
| +xx | xx |  | [Field Data](#field-data) (if NOT null) |

## Field Type
| Type byte | Type | Description |
|---|---|---|
| 00 | null | Null value |
| 01 | u8 | uint8 |
| 02 | u16 | uint16 |
| 03 | u32 | uint32 |
| 04 | u64 | uint64 |
| 05 | s8 | int8 |
| 06 | s16 | int16 |
| 07 | s32 | int32 |
| 08 | s64 | int64 |
| 09 | f32 | float |
| 0A | f64 | double |
| 0B | b8 | boolean |
| 0C | String | String |
| 0D | [Binary](#binary-data-type) | Raw binary data |
| 0E | [Object Ref](#object-reference) | Object reference |
| 10 | [Map](#map) | map |
| 11 | [Array](#array) | array |
| Extension types--> |
| 81 | Vec2 | Vec2 |
| 82 | Vec3 | Vec3 |
| 83 | Vec4 | Vec4 |
| 84 | Matrix4x4 | Matrix4x4 |


## Field Data (DEPRECATED)

### Plain old data types

| Field Type | Size | Format | Desc |
|---|---|---|---|
| null | 0B | | No data for null value |
| u32 | 4B | `00 00 00 00` | 4 bytes |
| String | \0 | StringValue\0 | Null terminated string |

### Binary data type (Deprecated)
| Field Type | Size | Format | Desc |
|---|---|---|---|
| Byte size | 8B | `xx xx xx xx xx xx xx xx` | `0` is valid size. |
| Flags | 8B | `xx xx xx xx xx xx xx xx` | BinaryBlob flags value. |
| xx | xx |  | Raw binary data. |

### Map
| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 8B | `xx xx xx xx xx xx xx xx` | Length of the **map** in bytes excluding **this**. Minimun is `4`. |
| +08 | 4B | `xx xx xx xx` | Total number of elements |
| +10 | \0 | keyName\0 | Key name |
| +xx | xx |  | [Field Value](#field-value) |
| +08 | \0 | keyName2\0 | Key name |
| +xx | xx |  | [Field Value](#field-value) |
|...|

### Array
| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 8B | `xx xx xx xx xx xx xx xx` | Size of **array** in bytes excluding **this**. Minimum is `4`. |
| +08 | 4B | | Total number of elements |
| +xx | xx |  | [Field Value](#field-value) 0 |
| +xx | xx |  | [Field Value](#field-value) 1 |
| +xx | xx |  | [Field Value](#field-value) 2 |
|...|



