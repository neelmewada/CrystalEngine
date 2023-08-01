# Package File Format Specification

The `Package` class is used by the engine to store & load assets & serialized objects. It stores the data in a binary file with `.casset` extension.

This document describes the file format specification of `.casset` files. The initial version number of this file format is 1.0.

### Terminology

| Column | Value | Description |
|---|---|---|
| Size | \0 | A null terminated string. |

# Format Specification

Spec tables with little endian format

## **Overview**
| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 8B | `00 50 41 43 4b 00 00 0a` | Magic Number: `. P A C K . . \n` |
| +08 | 8B | `00 00 01 00 00 00 00 00` | Version number: Major(u32) . Minor(u16) . Patch(u16) |
| +10 | 4B | `00 00 00 00` | File checksum only for the actual data |
| +14 | 8B | `00 00 00 00 00 00 00 00` | Data start offset (from start of file) |
| +1C | 8B | `xx xx xx xx xx xx xx xx` | Package UUID |
| +24 | \0 | `/Engine/SomeAssetFolder/SomeAsset\0` | Path/name of Package. |
| +xx | 4B | `00 00 00 02` | No. of external package dependencies **n** (v1.1) |
| +xx | \0 | `/Engine/Textures/SomeTex\0` | n-th package dependency. Repeat for n times. (v1.1) |
| +xx | \0 | `NormalMap\0` | Primary object/asset name (v1.2) |
| +xx | \0 | `/Code/System.CE::Texture\0` | Primary object/asset long type name (v1.2) |
| +xx | xx | | Newly added header fields |
| +xx | xx | | **Actual data. List of [Object Entries](#object-entry)** |
| +xx | 8B | `00 00 00 00 00 00 00 00` | End Of Object Entries List. (EOF) |

## **Object Entry**
| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 8B | `00 4f 42 4a 45 43 54 00` | Magic Number: `. O B J E C T .` |
| +08 | 8B | `xx xx xx xx xx xx xx xx` | Object Instance UUID |
| +10 | 1B | `01` | Is Asset? `0` or `1` |
| +11 | \0 | `TextureAtlas.Noise.MyNoiseTexture\0` | Virtual path to object within the package. |
| +xx | \0 | `/Engine/Core.CE::Texture\0` | Object class TypeName |
| +xx | 4B | `xx xx xx xx` | Length of **data list** in bytes. `0` is valid. |
| +04 | 4B | `xx xx xx xx` | Total number of fields |
| +08 | 8B | `xx xx xx xx xx xx xx xx` | Data start offset (from start of file) |
| +10 | \0 | 'SomeObjectName' | Object name (CE::Name) |
| +xx | \0 | `../SomeTexture.png` | Source asset relative path (only if an asset) |
| +xx | xx | | Newly added header fields |
| +xx | xx | | **Data: List of [Field Entries](#field-list-entry)** |
| +xx | 4B | `00 00 00 00` | End Of Field Entries List |
| +04 | 4B | `xx xx xx xx` | Data CRC checksum. Can be `0`. |

## **Field List Entry**
| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 4B | `f1 1f f1 1f` | Magic value. (v1.2) |
| +00 | \0 | `fieldName\0` | Name of the field. |
| +xx | \0 | `CE::Array\0` | Full type name of the field |
| +xx | 4B | `xx xx xx xx` | Field Data Size in bytes. `0` is a valid size. |
| +04 | xx | | [Field Data](#field-data) | 
| ... | ... | ... | ... |

## **Field Data**

Plain Old Data types are stored in their native formats. Ex: u32, f64, String, etc.

| Field Type | Size | Format |
|---|---|---|
| u32 | 4B | `00 00 00 00` |
| String | \0 | StringValue\0 |

### Array Field Data

Each array element is saved as a **Field Data** itself, making a recursive data structure.

| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | \0 | `/Engine/Core.CE::String\0` | Full type name of array elements |
| +xx | 4B | N | Number of elements |
| +04 | 4B | | Total data byte size. |
| +08 | xx | | Array element 0. Recursive **Field Data**. |
| ... | ... | ... | ... |
| +xx | xx | | Array element N - 1. Recursive **Field Data**. |

### Struct Field Data

When a field value is a struct type, then it's sub-fields are stored in the same format as [Field List Entry](#field-list-entry).

### Object Field Data

An object is always stored as a reference. i.e. we store the object's uuid, type name & the package name it belongs it (if it exists outside this package). If the object is NULL, then the UUID field will be 0 and all other fields would be omitted.

| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 8B | `xx xx xx xx xx xx xx xx` | Object Instance UUID |
| +08 | \0 | `CE::SomeClassName\0` | Object type name |
| +xx | \0 | `/Engine/MyAsset\0` | Package name it belongs to. |
| +xx | \0 | `TextureAtlas.Noise.PerlinMap\0` | Path within the package this object belongs to. |


