#pragma once

const char DescriptorParsing_DefaultPipeline_JSON[] = R"(
{
    "Name": "DefaultPipeline",
    "MainViewTag": "Camera",
    // Root pass (PassRequest)
    "RootPass": {
        "PassName": "DefaultPipelineRoot",
        "PassDefinition": "DefaultPipelineRoot",
        "ChildPasses": [ // [PassRequest] list
            {
                "PassName": "DepthPrePass",
                "PassDefinition": "DepthPrePassDefinition",
                "Connections": [],
                "ChildPasses": [] // No child passes
            },
            {
                "PassName": "OpaquePass",
                "PassDefinition": "OpaquePassDefinition",
                "Connections": [],
                "ChildPasses": [] // No child passes
            },
            {
                "PassName": "TransparentPass",
                "PassDefinition": "TransparentPassDefinition",
                "Connections": [ // INPUT connections should ALWAYS be defined inside "ChildPasses" and NOT in root pass definitions array.
                    { // Connect TransparentPass's Input to OpaquePass's Output like this:   OpaquePass[Color] -> TransparentPass[Color]
                        "LocalSlot": "Color",
                        "AttachmentRef": {
                            "Pass": "OpaquePass",
                            "Attachment": "Color"
                        }
                    },
                    { // Connect Color IO slot to PipelineOutput
                        "LocalSlot": "Color",
                        "AttachmentRef": {
                            "Pass": "$root",
                            "Attachment": "PipelineOutput"
                        }
                    }
                ],
                "ChildPasses": [] // No child passes
            }
        ]
    },
    // List of pass definitions
    "PassDefinitions": [
        // Root Pass (DefaultPipelineRoot): The render target image attachment is automaticall inferred, no need to manually define it.
        {
            "Name": "DefaultPipelineRoot",
            "PassClass": "ParentPass",
            "Slots": [
                // Final color output
                {
                    "Name": "PipelineOutput",
                    "SlotType": "InputOutput"
                }
            ]
        },
        //-----------------------------------------------------------------
        // Depth Pre-Pass Definition
        {
            "Name": "DepthPrePassDefinition",
            "PassClass": "RasterPass",
            "Slots": [
                {
                    "Name": "Output",
                    "SlotType": "Output",
                    "LoadStoreAction": {
                        "ClearValueDepth": 0.0,
                        "ClearValueStencil": 0,
                        "LoadAction": "Clear",
                        "StoreAction": "Store"
                    }
                }
            ],
            "ImageAttachments": [
                // Depth Stencil
                {
                    "Name": "DepthStencil",
                    "Lifetime": "Transient", // Transient or Imported
                    "SizeSource": { // Size should be same as PipelineOutput
                        "Source": {
                            "Pass": "$root", // Specials: $this, $parent, $root
                            "Attachment": "PipelineOutput"
                        },
                        "SizeMultipliers": [1, 1, 1] // (width, height, depth) multipliers
                    },
                    "ImageDescriptor": {
                        "BindFlags": ["DepthStencil"],
                        "Format": "D32_SFLOAT_S8_UINT"
                    },
                    "FallbackFormats": ["D24_UNORM_S8_UINT"]
                }
            ],
            "Connections": [
                // Connect Depth output slot to the DepthStencil image attachment
                {
                    "LocalSlot": "Output",
                    "AttachmentRef": {
                        "Pass": "$this",
                        "Attachment": "DepthStencil"
                    }
                }
            ],
            "PassData": {
                "DrawListTag": "depth",
                "ViewTag": "Camera"
            }
        },
        //-----------------------------------------------------------------
        // Opaque Pass Definition
        {
            "Name": "OpaquePassDefinition",
            "PassClass": "RasterPass",
            "Slots": [
                {
                    "Name": "Color",
                    "SlotType": "Output",
                    "AttachmentUsage": "RenderTarget",
                    "LoadStoreAction": {
                        "ClearValue": [
                            0.0, 0.0, 0.0, 0.0
                        ],
                        "LoadAction": "Clear",
                        "StoreAction": "Store"
                    }
                }
            ],
            "Connections": [
                // We ONLY specify Connections in Pass Definitions if we want to connect local slots to Image Attachments
                // If you want to connect slots with other slots, specify them under "ChildPasses" instead.
                {
                    "LocalSlot": "Color",
                    "AttachmentRef": {
                        "Pass": "$root",
                        "Attachment": "PipelineOutput" // Connect the pipeline's RenderTarget Image Attachment (PipelineOutput) to this pass's "Color" output slot.
                    }
                }
            ],
            "PassData": {
                "DrawListTag": "forward",
                "ViewTag": "Camera"
            }
        },
        //-----------------------------------------------------------------
        // Transparent Pass Definition
        {
            "Name": "TransparentPassDefinition",
            "PassClass": "RasterPass",
            "Slots": [
                {
                    "Name": "Color",
                    "SlotType": "InputOutput",
                    "LoadStoreAction": {
                        "LoadAction": "Load",
                        "StoreAction": "Store"
                    }
                }
            ],
            "Connections": [
                // No default connections here
            ],
            "PassData": {
                "DrawListTag": "forward",
                "ViewTag": "Camera"
            }
        }
    ]
}
)";
