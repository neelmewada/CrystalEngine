
# Crystal Package File Format Specification

The `Package` class is used by the engine to store & load assets & serialized objects. It stores the data in a binary file with `.cpak` extension. cpak files are loaded by engine in 2 steps. In 1st step, a table of content is populated storing each entry with it's location. In 2nd step, the data is fully loaded.

This document describes the file format specification of `.cpak` files.


# Format Specification

## **Header**

Each cpak file will start with a header. The header consists of following fields:

### Magic Number (8 bytes)

The file always starts with the following magic number.

`00`  `43`  `50`  `41`  `4b`  `00`  `00`  `0a`  
`.` `C` `P` `A` `K` `.` `.` `\n`

### Version Number (4 bytes)

The file format version number. First 2 bytes are major, next 2 bytes are minor.

`00` `00` `01` `00` --- v0.1

### File CRC (4 bytes)

A 4 byte unsigned integer that stores the checksum of the data (excluding all the header fields)

### Data Offset (4 bytes)

Offset to the starting location of actual data in this file calculated from the start of the file.

### New header fields

Any new header fields can be added after the data offset for backwards compatibility.

---
## **Object Entry**

A serialized object entry. This is where the serialized representation of an object is stored.

It consists of following fields.

### Magic Number (8 bytes)

An 'entry' always starts with a magic number denoting what kind of entry this is. Right now, only Object Entries are supported.

`00` `4f` `42` `4a` `45` `43` `54` `00` <br>
 `.` `O` `B` `J` `E` `C` `T` `.`

### Object UUID (8 bytes)

A 64 bit unique ID (uuid) of the object instance.

`92` `32` `42` `19` `71` `01` `81` `98`

### Is Asset? (1 byte)

A boolean (0 or 1) denoting if this object is an asset

### Virtual Asset Path (String) (IF asset == 1)

Valid only if IsAsset (previous byte) is true! Otherwise a `00` byte.
It is a string denoting the virtual path to the asset. Example:

`2f` `45` `6e` `67` `69` `6e` ... `72` `65` `00`

/Engine/Assets/Texture.asset

### Object Class TypeName (String)

A null `00` terminated string that stores the full type name of the object. Example:

`43` `45` `3a` `3a` `53` `63` `65` `6e` `65` `00` <br>
 `C` `E` `:` `:` `S` `c` `e` `n` `e` `.`

### Length (4 bytes)

The length of actual data stored in this entry, excluding previous fields (typename, uuid) & the 32 bit checksum. `0` is a valid length, meaning no data is stored.

### **Data**

Actual serialized object data. Refer to the [Object Entry Data](#object-entry-data) section for more details.


### Object Entry CRC (4 bytes)

A 32-bit checksum of above data.


---
## **Object Entry Data**

Each object entry has a Data section as described earlier. It's format is as follows:

### **Fields List**

#### Entry size (4 bytes)

A 32-bit unsigned integer denoting the size of 'this' field entry excluding itself. `0` is a valid size, which means we have reached the end of field list.

#### Field Name

A null `00` terminated string that stores the name of the field.

#### Field Type Name

A null `00` terminated string that stores the full type name of the field.

---
## **Spec Tables**

Spec tables with little endian format

### **Overview**
| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | 8B | `00 43 50 41 4b 00 00 0a` | Magic Number: `. C P A K . \n` |
| +08 | 4B | `00 00 01 00` | Version number: Major.Minor (2 bytes each) |
| +0C | 4B | `00 00 00 00` | File checksum only for the actual data |
| +10 | 4B | `00 00 00 00` | Data start offset (from start of file) |
| +xx | xx | | Newly added header fields |
| +xx | xx | | **Actual data. List of [Object Entries](#object-entry-1)** |
| +xx | 8B | `00 00 00 00 00 00 00 00` | End Of Object Entries List. |

### **Object Entry**
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

### **Object Entry Field List**
| Offset | Size | Value | Description |
|---|---|---|---|
| +00 | \0 | `fieldName\0` | Name of the field. |
| +xx | \0 | `CE::Array\0` | Full type name of the field |
| +xx | 4B | `xx xx xx xx` | Field Data Size in bytes. `0` is a valid size. |
| +04 | xx | | Field Data |

