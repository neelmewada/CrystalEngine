
registerFileType((fileExt, filePath, fileData) => {
    return true
    // Check for wav extension
    if (fileExt == 'casset' || fileExt == "temp") {
        const headerArray = fileData.getBytesAt(0, 8);
        const matchArray = [ 0x00, 0x42, 0x55, 0x4e, 0x44, 0x4c, 0x45, 0x00 ]; // . B U N D L E .
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
    addRow('Header', ".BUNDLE.", "Bundle header entry")
    
    // addDetails(() => {
    //     for (let i = 0; i < 8; i++) {
    //         read(1);
    //         var desc = "" + String.fromCharCode(getNumberValue());
    //         addRow('Bit ' + i, getHex0xValue(), desc);
    //     }
    // });

    read(4);
    addRow('Checksum', getHex0xValue(), 'Data Checksum CRC');

    read(4);
    addRow('Major', getNumberValue(), 'Major version (u32)');
    read(4);
    addRow('Minor', getNumberValue(), 'Minor version (u32)');
    read(4);
    addRow('Patch', getNumberValue(), "Patch version (u32)")

    read(8);
    addRow('Schema Table Offset', getNumberValue(), 'Schema Table offset from start of file');

    read(8);
    addRow('Serialize Data Offset', getNumberValue(), 'Serialize Data offset from start of file');

    read(16);
    //addRow('Bundle UUID', getHexValue(), "Bundle UUID");

    readUntil(0);
    addRow('Bundle Name', getStringValue(), "Path of the bundle in project/engine");
    read(1);

    read(4);
    var numDeps = getNumberValue();
    addRow('Num External Dependencies', numDeps, "Number of external dependencies");

    for (let i = 0; i < numDeps; i++) {
        readUntil(0);
        addRow('Bundle Dep ' + i, getStringValue(), "External bundle dependency " + i);
    }

    read(1);
    addRow('Is Cooked?', getNumberValue(), "True only if value > 0");

    read(8);
    var schemaSize = getNumberValue()
    addRow('Size of Schema Table', schemaSize, 'Size includes the size field itself and end of schema marker')

    read(schemaSize - 8)
    // addRow('Schema', 'Schema')

    read(8);
    addRow('EOF', '0', 'End Of File')

});

