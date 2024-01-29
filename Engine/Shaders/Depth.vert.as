; SPIR-V
; Version: 1.0
; Generator: Google spiregg; 0
; Bound: 35
; Schema: 0
               OpCapability Shader
               OpMemoryModel Logical GLSL450
               OpEntryPoint Vertex %VertMain "VertMain" %in_var_POSITION %gl_Position
               OpSource HLSL 600
               OpName %type__PerViewData "type._PerViewData"
               OpMemberName %type__PerViewData 0 "viewMatrix"
               OpMemberName %type__PerViewData 1 "viewProjectionMatrix"
               OpMemberName %type__PerViewData 2 "projectionMatrix"
               OpName %_PerViewData "_PerViewData"
               OpName %type_ConstantBuffer_ObjectData "type.ConstantBuffer.ObjectData"
               OpMemberName %type_ConstantBuffer_ObjectData 0 "modelMatrix"
               OpName %_ObjectData "_ObjectData"
               OpName %in_var_POSITION "in.var.POSITION"
               OpName %VertMain "VertMain"
               OpDecorate %gl_Position BuiltIn Position
               OpDecorate %in_var_POSITION Location 0
               OpDecorate %_PerViewData DescriptorSet 1
               OpDecorate %_PerViewData Binding 0
               OpDecorate %_ObjectData DescriptorSet 5
               OpDecorate %_ObjectData Binding 1
               OpMemberDecorate %type__PerViewData 0 Offset 0
               OpMemberDecorate %type__PerViewData 0 MatrixStride 16
               OpMemberDecorate %type__PerViewData 0 RowMajor
               OpMemberDecorate %type__PerViewData 1 Offset 64
               OpMemberDecorate %type__PerViewData 1 MatrixStride 16
               OpMemberDecorate %type__PerViewData 1 RowMajor
               OpMemberDecorate %type__PerViewData 2 Offset 128
               OpMemberDecorate %type__PerViewData 2 MatrixStride 16
               OpMemberDecorate %type__PerViewData 2 RowMajor
               OpDecorate %type__PerViewData Block
               OpMemberDecorate %type_ConstantBuffer_ObjectData 0 Offset 0
               OpMemberDecorate %type_ConstantBuffer_ObjectData 0 MatrixStride 16
               OpMemberDecorate %type_ConstantBuffer_ObjectData 0 RowMajor
               OpDecorate %type_ConstantBuffer_ObjectData Block
        %int = OpTypeInt 32 1
      %int_0 = OpConstant %int 0
      %float = OpTypeFloat 32
    %float_1 = OpConstant %float 1
      %int_1 = OpConstant %int 1
    %v4float = OpTypeVector %float 4
%mat4v4float = OpTypeMatrix %v4float 4
%type__PerViewData = OpTypeStruct %mat4v4float %mat4v4float %mat4v4float
%_ptr_Uniform_type__PerViewData = OpTypePointer Uniform %type__PerViewData
%type_ConstantBuffer_ObjectData = OpTypeStruct %mat4v4float
%_ptr_Uniform_type_ConstantBuffer_ObjectData = OpTypePointer Uniform %type_ConstantBuffer_ObjectData
    %v3float = OpTypeVector %float 3
%_ptr_Input_v3float = OpTypePointer Input %v3float
%_ptr_Output_v4float = OpTypePointer Output %v4float
       %void = OpTypeVoid
         %21 = OpTypeFunction %void
%_ptr_Uniform_mat4v4float = OpTypePointer Uniform %mat4v4float
%_PerViewData = OpVariable %_ptr_Uniform_type__PerViewData Uniform
%_ObjectData = OpVariable %_ptr_Uniform_type_ConstantBuffer_ObjectData Uniform
%in_var_POSITION = OpVariable %_ptr_Input_v3float Input
%gl_Position = OpVariable %_ptr_Output_v4float Output
   %VertMain = OpFunction %void None %21
         %23 = OpLabel
         %24 = OpLoad %v3float %in_var_POSITION
         %25 = OpCompositeExtract %float %24 0
         %26 = OpCompositeExtract %float %24 1
         %27 = OpCompositeExtract %float %24 2
         %28 = OpCompositeConstruct %v4float %25 %26 %27 %float_1
         %29 = OpAccessChain %_ptr_Uniform_mat4v4float %_ObjectData %int_0
         %30 = OpLoad %mat4v4float %29
         %31 = OpMatrixTimesVector %v4float %30 %28
         %32 = OpAccessChain %_ptr_Uniform_mat4v4float %_PerViewData %int_1
         %33 = OpLoad %mat4v4float %32
         %34 = OpMatrixTimesVector %v4float %33 %31
               OpStore %gl_Position %34
               OpReturn
               OpFunctionEnd
