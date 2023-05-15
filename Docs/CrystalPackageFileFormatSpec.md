# Crystal Package File Format Specification

The `Package` class is used by the engine to store & load assets & serialized objects. It stores the data in a binary file with `.cpak` extension. cpak files are loaded by engine in 2 steps. In 1st step, a table of content is populated storing each entry with it's location. In 2nd step, the data is fully loaded.

This document describes the file format specification of `.cpak` files.


# Format Specification

Spec tables with little endian format

## **Overview**
| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 8B | `00 43 50 41 4b 00 00 0a` | Magic Number: `. C P A K . \n` |
| +08 | 4B | `00 00 01 00` | Version number: Major.Minor (2 bytes each) |
| +0C | 4B | `00 00 00 00` | File checksum only for the actual data |
| +10 | 4B | `00 00 00 00` | Data start offset (from start of file) |
| +xx | xx | | Newly added header fields |
| +xx | xx | | **Actual data. List of [Object Entries](#object-entry-1)** |
| +xx | 8B | `00 00 00 00 00 00 00 00` | End Of Object Entries List. |

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
| +00 | \0 | `fieldName\0` | Name of the field. |
| +xx | \0 | `CE::Array\0` | Full type name of the field |
| +xx | 4B | `xx xx xx xx` | Field Data Size in bytes. `0` is a valid size. |
| +04 | xx | | Field Data |

