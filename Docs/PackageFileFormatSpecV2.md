# Package File Format Specification

The `Package` class is used by the engine to store & load assets & serialized objects. It stores the data in a binary file with `.casset` extension.

This document describes the file format specification of `.casset` files. The initial version number of this file format is 1.0.

# Format Specification

Spec tables with big endian format

### Basic Types

| Type | Value | Description |
|---|---|---|
| String | `T e x t 00` | Null terminated string |

## **Overview**
| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 8B | `00 50 41 43 4b 00 00 0a` | Magic Number: `. P A C K . . \n` |
| +08 | 8B | 2.0 | Version number: Major(u32) . Minor(u32) |
| +10 | 4B | `00 00 00 00` | Data checksum |
| +14 | 8B | `00 00 00 00 00 00 00 00` | Data start offset (from start of file) |
| +1C | 8B | `xx xx xx xx xx xx xx xx` | Package UUID |
| +24 | \0 | `/Game/Assets/SomeAsset\0` | Path/name of Package. |
| +xx | 4B | `01 00 00 00` | No. of external package dependencies **n** |
| +xx | \0 | `/Engine/Assets/SomeTex\0` | n-th package dependency. Repeat for n times. |
| +01 | 1B | `00/01` | Is cooked? |
| +xx | xx | | Newly added header fields |
| +xx | xx | | **Actual Data. List of [Object Entries](#object-entry)** |
| +xx | 8B | `00 00 00 00 00 00 00 00` | End Of Object Entries List. (EOF) |

## **Object Entry**
| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 8B | `00 4f 42 4a 45 43 54 00` | Magic Number: `. O B J E C T .` |
| +08 | 8B | `xx xx xx xx xx xx xx xx` | Object Instance UUID |
| +10 | 1B | `01` | Is Asset? `0` or `1` |
| +11 | \0 | `TextureAtlas.Noise.MyNoiseTexture\0` | Virtual path to object within the package. |
| +xx | \0 | `/Engine/Core.CE::Texture\0` | Object class TypeName |
| +08 | 8B | `xx xx xx xx xx xx xx xx` | Data start offset (from start of file) |
| +10 | \0 | `SomeObjectName\0` | Object name (CE::Name) |
| +xx | \0 | `/Textures/SomeTexture.png` | Source asset path relative to project (exists only if **Is Asset?**) |
| +xx | xx | | Newly added header fields |
| +xx | xx | | A **[Map](#map)** of fields. |
| +xx | 4B | `00 00 00 00` | End Of Field Entries List |
| +04 | 4B | `xx xx xx xx` | Data CRC checksum. Can be `0`. |

## Map
| Offset | Size | Value | Description |
|---|---|---|---|
| +xx | 4B | `xx xx xx xx` | Length of the **map** in bytes excluding **this**. `0` is valid. |
| +04 | 4B | `xx xx xx xx` | Total number of elements |
| +08 | \0 | keyName\0 | Key name |
| +xx | xx |  | [Field Value](#field-value) |
| +08 | \0 | keyName2\0 | Key name |
| +xx | xx |  | [Field Value](#field-value) |
|...|

## **Field Value**
| Offset | Size | Value | Description |
|---|---|---|---|
| +xx | 1B | `00` | [Field Type](#field-type) |
| +xx | xx |  | [Field Data](#field-data) |

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
| 0D | Binary | Raw binary data |
| 10 | [Map](#map) | map |
| 11 | [Array](#array) | array |

## Field Data

Plain old data types:

| Field Type | Size | Format |
|---|---|---|
| u32 | 4B | `00 00 00 00` |
| String | \0 | StringValue\0 |

Binary data type:
| Field Type | Size | Format |
|---|---|---|
| Byte size | 8B | `xx xx xx xx xx xx xx xx` |
| xx | xx | Raw binary data |


## Array
| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 4B | `xx xx xx xx` | Length of **array** in bytes excluding **this**. `0` is valid. |
| +04 | 4B | | Total number of elements |
| +xx | xx |  | [Field Value](#field-value) |
| +xx | xx |  | [Field Value](#field-value) #2 |
|...|

## Object Reference fields

Object reference fields are stored as an **array**.

| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 4B | `xx xx xx xx` | Length of **array** in bytes excluding **this**. `0` is valid. |
| +04 | 4B | 5 | Total number of elements |
| +08 | 8B | `xx xx xx xx xx xx xx xx` | Object UUID (u64 [Field Value](#field-value)) |
| +10 | xx | `/Code/Core.CE::SomeClass\0` | Object class name (String [Field Value](#field-value)) |
| +xx | 8B | `xx xx xx xx xx xx xx xx` | Package UUID it belongs to (u64 [Field Value](#field-value)) |
| +08 | xx | `/Game/Assets/MyTexture\0` | Package name/path it belongs to (String [Field Value](#field-value)) |


