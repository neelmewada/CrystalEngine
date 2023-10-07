
registerFileType((fileExt, filePath, fileData) => {
    // Check for wav extension
    if (fileExt == 'casset' || fileExt == "temp") {
        const headerArray = fileData.getBytesAt(0, 8);
        const matchArray = [ 0x00, 0x50, 0x41, 0x43, 0x4b, 0x00, 0x00, 0x0a ];
        for (let i = 0; i < matchArray.length; i++) {
            if (matchArray[i] != headerArray[i])
                return false;
        }
        return true;
    }
    return false;
});

registerParser(() => {
    read(8);
    addRow('Header', ".PACK..\n", "Package header entry")
    addDetails(() => {
        for (let i = 0; i < 8; i++) {
            read(1);
            var desc = "" + String.fromCharCode(getNumberValue());
            addRow('Bit ' + i, getHex0xValue(), desc);
        }
    });

    read(4);
    addRow('Major', getNumberValue(), 'Major version (u32)');
    read(4);
    addRow('Minor', getNumberValue(), 'Minor version (u32)');

    read(4);
    addRow('Checksum', getHex0xValue(), 'Data Checksum CRC');

    read(8);
    addRow('Data Start Offset', getNumberValue(), 'Data start offset from start of file');

    read(8);
    addRow('Package UUID', getNumberValue(), "Package UUID");

    readUntil(0);
    addRow('Package Name', getStringValue(), "Path of the package in project/engine");
    read(1);

    read(4);
    var numDeps = getNumberValue();
    addRow('Num External Dependencies', numDeps, "Number of external dependencies");

    for (let i = 0; i < numDeps; i++) {
        readUntil(0);
        addRow('Package Dep ' + i, getStringValue(), "External package dependency " + i);
    }

    read(1);
    addRow('Is Cooked?', getNumberValue(), "True only if value > 0");

    const objectHeader = [ 0x00, 0x4f, 0x42, 0x4a, 0x45, 0x43, 0x54, 0x00 ];
    var match = true;
    
    while (match)
    {
        var readHeader = [];
        var headerString = ""
        match = true

        for (let i = 0; i < objectHeader.length; i++) {
            read(1);
            readHeader.push(getNumberValue());
            headerString += getHex0xValue() + " "

            if (getNumberValue() != objectHeader[i])
                match = false
        }

        addRow('Object header', headerString, "Should match to .OBJECT. for valid entry. 0 is EOF.");

        if (!match)
        {
            break;
        }

        parseObject();
    }
});

function parseObject() {
    read(8)
    addRow('Object UUID', getNumberValue(), "Object UUID (u64)")

    read(1)
    var isAsset = getNumberValue() > 0
    addRow('Is Asset?', getNumberValue(), "True if value > 0")

    readUntil(0)
    addRow('Path within package', getStringValue(), "Path to object within the package")
    read(1)

    readUntil(0)
    addRow('Object Class', getStringValue(), "Object class TypeName")
    read(1)

    readUntil(0)
    addRow('Object Name', getStringValue())
    read(1)

    if (isAsset) {
        readUntil(0)
        addRow('Source Asset Path', getStringValue(), "Path to the source asset file")
        read(1)
    }

    read(8)
    addRow('Data Start Offset', getNumberValue(), "Data start offset from start of file")

    read(1)
    addRow('Field Type: Map', getHex0xValue(), "Should always be 0x10")

    parseMap("Map: Object Fields")

    read(4)
    addRow('Checksum', getNumberValue(), "Data checksum")
}

function parseMap(title = "Map") {
    read(8);
    var dataSize = getNumberValue();
    addRow('Data Size', dataSize, "Total data byte size of this map");

    read(dataSize);
    addRow(title)
    addDetails(() => {
        read(4);
        var numEntries = getNumberValue()
        addRow('Num entries', numEntries, "Number of entries in map");

        for (let i = 0; i < numEntries; i++) {
            readUntil(0)
            addRow("Field Name", getStringValue())
            read(1)
            
            parseFieldValue()
        }
    });
}

function parseArray(title = "Array") {
    read(8);
    var dataSize = getNumberValue();
    addRow('Data Size', dataSize, "Total data byte size of this array");

    read(dataSize);
    addRow(title)
    addDetails(() => {
        read(4);
        var numEntries = getNumberValue();
        addRow('Num entries', numEntries, "Number of entries in array");

        for (let i = 0; i < numEntries; i++) {
            parseFieldValue()
        }
    });
}

function parseFieldValue() {
    read(1);
    var fieldType = getNumberValue()
    addRow('Field Type', getHex0xValue(), "Field type byte")

    switch (fieldType) {
        case 0x00:
            addRow("NULL");
            break;
        case 0x01:
            read(1)
            addRow('u8', getNumberValue());
        case 0x02:
            read(2)
            addRow('u16', getNumberValue());
            break;
        case 0x03:
            read(4)
            addRow('u32', getNumberValue());
            break;
        case 0x04:
            read(8)
            addRow('u64', getNumberValue());
            break;
        case 0x05:
            read(1)
            addRow('s8', getNumberValue());
        case 0x06:
            read(2)
            addRow('s16', getNumberValue());
            break;
        case 0x07:
            read(4)
            addRow('s32', getNumberValue());
            break;
        case 0x08:
            read(8)
            addRow('s64', getNumberValue());
            break;
        case 0x09:
            read(4)
            addRow('f32', getFloatNumberValue());
            break;
        case 0x0A:
            read(8)
            addRow('f64', getFloatNumberValue());
            break;
        case 0x0B:
            read(1)
            addRow('b8', getNumberValue() > 0 ? true : false);
            break;
        case 0x0C:
            readUntil(0)
            addRow('String', getStringValue());
            read(1)
            break;
        case 0x0D:
            read(8)
            const byteSize = getNumberValue()
            addRow('Binary Data (Byte Size)', byteSize)
            
            read(byteSize)
            addDetails(() => {
                read(byteSize)
                addMemDump()
            });
            break;
        case 0x0E:
            read(8)
            addRow('Object UUID', getNumberValue())
            read(8)
            addRow('Package UUID', getNumberValue())
            break;
        case 0x10:
            parseMap()
            break;
        case 0x11:
            parseArray()
            break;
        case 0x81:
            parseVector(2)
            break;
        case 0x82:
            parseVector(3)
            break;
        case 0x83:
            parseVector(4)
            break;
    }
}

function parseVector(numComponents = 4) {
    var text = "("

    for (let i = 0; i < numComponents; i++) {
        read(4)
        if (i > 0)
        text += ", "
        text += getFloatNumberValue()
    }

    text += ")"

    addRow("Vector" + numComponents, text)
}
