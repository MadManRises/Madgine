#ifndef __cplusplus

//ByteAddressBuffer memory[128] : register(t5);

template <typename T>
struct ArrayPtr{
	uint offset;
};

template <>
struct ArrayPtr<float4x4>{
	float4x4 operator[](uint address){
		//return memory[buffer()].Load<float4x4>(offset() + address);
		return float4x4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
	}

	operator bool(){
		return buffer() != 0;
	}

	uint offset() {
		return data.x;
	}

	uint buffer() {
		return data.y;
	}

	uint2 data;
};

#endif