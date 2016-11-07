
#pragma once

// ��� ���� ������

typedef long long						ssh_l;
typedef unsigned long long				ssh_u;
typedef unsigned char					ssh_b;
typedef unsigned short					ssh_w;
typedef unsigned long					ssh_d;
typedef unsigned int					ssh_i;
typedef const char*						ssh_ccs;
typedef const wchar_t*					ssh_cws;
typedef char							ssh_cs;
typedef char*							ssh_pcs;
typedef wchar_t							ssh_ws;
typedef wchar_t*						ssh_pws;

#define SSH_SDK_VERSION					6
// �������

#define SSH_MEMZERO(dst, len)			memset(dst, 0, len)
#define SSH_DEL(p)						{if(p) delete p; p = nullptr;}
#define SSH_DEL_A(p)					(if(p) delete [] p; p = nullptr;)
#define SSH_REL(p)						{if(p) p->release(); p = nullptr;}
#define SSH_DYNCREATE(cls)				public:\
										void* operator new(ssh_u sz, cls** obj, const String& name) \
										{\
											return (cls*)Base::operator new(sz, (Base**)obj, name, String(L#cls)); \
										}
#define SSH_NEW_DECL(cls, SIZE)			static MemArray<cls, SIZE>* get_MemArray##cls() { static MemArray<cls, SIZE> memarray; return &memarray; } \
										void* operator new(size_t sz) {return (void*)get_MemArray##cls()->Alloc();} \
										void operator delete(void* p) { get_MemArray##cls()->Free((cls*)p); } \
										static void reset() { get_MemArray##cls()->Reset(); }
#define SSH_IS_PTR(t)					(bool)std::is_pointer<t>()
#define SSH_CAST(v1, v2)				static_cast<v1>(v2)
#define SSH_FFL							__FUNCTIONW__, __FILEW__, __LINE__

#define SSH_CLAMP(val, low, high)		(val < low ? low : (val > high ? high : val))
#define SSH_THROW(msg, ...)				throw Exception(SSH_FFL, msg, __VA_ARGS__)
#define SSH_FAULT(code, except)			log->fault(code, SSH_FFL, except)
// ���������
#define SSH_EPSILON						(1e-06)
#define SSH_EPSILON2					(SSH_EPSILON * SSH_EPSILON)
#define SSH_PI							3.14f
#define SSH_HALF_PI						(0.5f * SSH_PI)
#define SSH_RGBA_A(rgba)				((rgba) >> 24)
#define SSH_RGBA_B(rgba)				(((rgba) >> 16) & 0xff)
#define SSH_RGBA_G(rgba)				(((rgba) >> 8) & 0xff)
#define SSH_RGBA_R(rgba)				((rgba) & 0xff)
#define SSH_RGBA(r, g, b, a)			((((a) << 24) | ((b) << 16) | ((g) << 8) | (r)))
#define SSH_DEG2RAD						(SSH_PI / 180.0f)
#define SSH_RAD2DEG						(180.0f / SSH_PI)
#define SSH_

#define cp_ansi							L"windows-1251"
#define cp_utf							L"utf-16le"
