# Crystal Package File Format Specification

The `Package` class is used by the engine to store & load assets & serialized objects. It stores the data in a binary file with `.cpak` extension. cpak files are loaded by engine in 2 steps. In 1st step, a table of content is populated storing each entry with it's location. In 2nd step, the data is fully loaded.

This document describes the file format specification of `.cpak` files.


# Format Specification

Spec tables with little endian format

## **Overview**
| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 8B | `00 43 50 41 4b 00 00 0a` | Magic Number: `. C P A K . . \n` |
| +08 | 4B | `00 00 01 00` | Version number: Major.Minor (2 bytes each) |
| +0C | 4B | `00 00 00 00` | File checksum only for the actual data |
| +10 | 4B | `00 00 00 00` | Data start offset (from start of file) |
| +14 | 8B | `xx xx xx xx xx xx xx xx` | Package UUID |
| +1C | \0 | `AssetPackageName\0` | Name of Package object. |
| +xx | xx | | Newly added header fields |
| +xx | xx | | **Actual data. List of [Object Entries](#object-entry)** |
| +xx | 8B | `00 00 00 00 00 00 00 00` | End Of Object Entries List. (EOF) |

## **Object Entry**
| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 8B | `00 4f 42 4a 45 43 54 00` | Magic Number: `. O B J E C T .` |
| +08 | 8B | `xx xx xx xx xx xx xx xx` | Object Instance UUID |
| +10 | 1B | `01` | Is Asset? `0` or `1` |
| +11 | \0 | `/Engine/Assets/MyTexture.asset\0` | Virtual path to asset. Null `00` if not asset. |
| +xx | \0 | `CE::Texture\0` | Object class TypeName |
| +xx | 4B | `xx xx xx xx` | Length of actual data only. `0` is valid. |
| +04 | xx |  | **Data: List of [Field Entries](#object-entry-field-list)** |
| +xx | 4B | `00 00 00 00` | End Of Field Entries List |
| +xx | 4B | `xx xx xx xx` | Data CRC checksum. `0` if no data. |

## **Object Entry Field List**
| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 4B | `xx xx xx xx` | Total number of fields |
| +04 | \0 | `fieldName\0` | Name of the field. |
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

Each array element is saved as a Field Data itself, making a recursive data structure.

| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | \0 | | Full type name of array elements |
| +xx | 4B | N | Number of elements |
| +xx | xx | | Array element 0. Recursive **Field Data**. |
| ... | ... | ... | ... |

### Struct Field Data

When a field value is a struct type, then it is stored in the same format as [Object Entry Field List](#object-entry-field-list).


