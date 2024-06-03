# Bundle File Format Specification

The `Bundle` class is used by the engine to store & load assets & serialized objects. It stores the data in a binary file with `.casset` extension.

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
| +00 | 8B | `00 42 55 4e 44 4c 45 00` | Magic Number: `. B U N D L E .` |
| +08 | 4B | 2 | Major Version (u32) |
| +0C | 4B | 0 | Minor Version (u32) |
| +10 | 4B | `00 00 00 00` | Data checksum |
| +14 | 8B | `00 00 00 00 00 00 00 00` | Data start offset (from start of file) |
| +1C | 8B | `xx xx xx xx xx xx xx xx` | Bundle UUID |
| +24 | \0 | `/Game/Assets/SomeAsset\0` | Path/name of Bundle. |
| +xx | 4B | `01 00 00 00` | No. of external Bundle dependencies **n** |
| +xx | 8B | `xx xx xx xx xx xx xx xx` | n-th Bundle dependency. Repeat for n times. |
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


## Field Data

### Plain old data types

| Field Type | Size | Format | Desc |
|---|---|---|---|
| null | 0B | | No data for null value |
| u32 | 4B | `00 00 00 00` | 4 bytes |
| String | \0 | StringValue\0 | Null terminated string |

### Binary data type
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

### Object Reference

| Offset | Size | Value | Description |
|---|---|---|---|
| +08 | 8B | `xx xx xx xx xx xx xx xx` | Object UUID (u64 [Field Value](#field-value)) |
| +10 | 8B | `xx xx xx xx xx xx xx xx` | Bundle UUID it belongs to (u64 [Field Value](#field-value)) |

