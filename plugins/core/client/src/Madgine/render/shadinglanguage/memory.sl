#ifndef __cplusplus

ByteAddressBuffer memory[128] : register(t5);

template <typename T>
struct ArrayPtr{
	uint offset;
};

template <>
struct ArrayPtr<float4x4>{
	float4x4 operator[](uint address){
		return memory[buffer].Load<float4x4>(offset + address);
	}

	operator bool(){
		return buffer != 0;
	}

	uint offset;
	uint buffer;
};

#endif