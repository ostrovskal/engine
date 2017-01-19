
#pragma once

#include "ssh_str.h"

namespace ssh
{
	template <typename T> class Bar;

	template <typename T, bool> struct release_node { static void release(const T& t) { static_assert(false, "release_node invalid!"); } };
	template <typename T> struct release_node < T, false > { static void release(const T& t) { t.~T(); } static T dummy() { return T(); } };
	template <typename T> struct release_node < T, true > { static void release(const T& t) { delete t; } static T dummy() { return nullptr; } };

	#define SSH_RELEASE_NODE(T, V)	release_node<T, SSH_IS_PTR(T)>::release(V)
	#define SSH_DUMMY(T)			release_node<T, SSH_IS_PTR(T)>::dummy()

	class SSH Lock
	{
	public:
		Lock() { EnterCriticalSection(get_section()); }
		~Lock() { LeaveCriticalSection(get_section()); }
	protected:
		static CRITICAL_SECTION* get_section()
		{
			static CRITICAL_SECTION section;
			static bool init(false);
			if(!init) { InitializeCriticalSection(&section); init = true; }
			return &section;
		}
	};

	class SSH Base
	{
		void* operator new(ssh_u sz);
	public:
		// �������� ��������� ������ ��� ������
		void* Base::operator new(ssh_u sz, Base** obj, String name, String type)
		{
			// 1. ���������� ������ ��� ����������?
			Base* n(root);
			while(n)
			{
				if(n->name() == name && n->type() == type)
				{
					// ����������, ����� ����������� �������
					n->ref++;
					*obj = n;
					return nullptr;
				}
				n = n->next;
			}
			// �� ����������, ������� �����
			*obj = (Base*)::operator new(sz);
			(*obj)->ref = 1;
			(*obj)->next = root;
			(*obj)->nm.init(); (*obj)->nm = std::move(name);
			(*obj)->tp.init(); (*obj)->tp = std::move(type);
			root = *obj;
			return (void*)*obj;
		}
		// ����������� ������
		void release()
		{
			if(--ref == 0)
			{
				Base* p(nullptr);
				auto n(root);
				while(n && n != this) p = n, n = n->next;
				if(p) p->next = n->next; else root = root->next;
				delete this;
			}
		}
		// ��������� �������
		void add_ref() { ref++; }
		// ������� ���
		String name() const { return nm; }
		// ������� ���
		String type() const { return tp; }
	protected:
		Base() : nm(nullptr), tp(nullptr) {}
		// ����������
		virtual ~Base() {  }
		// ����� ������
		ssh_l ref;
		// ���
		String nm;
	private:
		// ��� ����
		String tp;
		// ������ ������
		static Base* root;
		// ��������� � ������
		Base* next;
	};

	class SSH Bits
	{
	public:
		// �����������
		Bits() : value(0) {}
		// ����������� �� ��������
		Bits(ssh_l v) : value(v) {}
		// ����������� �����
		Bits(const Bits& src) : value(src.value) {}
		// ��������
		// ������������
		const Bits& operator = (const Bits& src) { value = src.value; return *this; }
		const Bits& operator = (const ssh_l src) { value = src; return *this; }
		// ����������
		operator ssh_l() const { return value; }
		// ����������
		bool operator == (const ssh_l v) const { return (value == v); }
		bool operator != (const ssh_l v) const { return (value != v); }
		// �������
		// ����������
		void set(const ssh_l v) { value = v; }
		// ��������
		void clear() { value = 0; }
		// ��������
		void add_flags(const ssh_l v) { value |= v; }
		// ���������
		void del_flags(const ssh_l v) { value &= ~v; }
		// ���������� ���
		void set_bit(ssh_l bit) { _bittestandset64(&value, bit); }
		// �������� ���
		void clear_bit(ssh_l bit) { _bittestandreset64(&value, bit); }
		// ���������� �� ��������
		void set_bit(ssh_l bit, bool is) { is ? set_bit(bit) : clear_bit(bit); }
		// ������ ?
		bool is_empty() const { return (value == 0); }
		// �������� �� ���
		bool test_bit(ssh_l bit) const { return (_bittest64(&value, bit) != 0); }
		// �������� �� ��������� ���
		bool test_flags(const ssh_l test) const { return ((value & test) == test); }
		// �������� �� ������� ���� ������ ��������������
		bool test_any(const ssh_l test) const { return ((value & test) != 0); }
		// �����
		ssh_u total_bits() const { return 64; }
		// ���������� �������������
		ssh_u total_set() const { return __popcnt64(value); }
		// "����������" ����
		ssh_u reverse() const { return ssh_rev_bits((int)(value >> 32)) | ((ssh_u)(ssh_rev_bits((int)value)) << 32); }
		// ��������
		ssh_l value;
	};

	template <typename T = ssh_u> class Pts
	{
	public:
		// ������������
		Pts() : x(0), y(0) {}
		Pts(const T* ptr) { x = ptr[0]; y = ptr[1]; }
		Pts(const T& X, const T& Y) : x(X), y(Y) {}
		Pts(const Pts<T>& pt) : x(pt.x), y(pt.y) {}
		Pts(const POINT& pt) : x(pt.x), y(pt.y) {}
		Pts(const Bar<T> b) : x(b.x), y(b.y) {}
		// ��������
		Pts<T> operator - (const Pts<T>& p) const { return Pts<T>(x - p.x, y - p.y); }
		Pts<T> operator - (const T& i) const { return Pts<T>(x - i, y - i); }
		Pts<T> operator + (const Pts<T>& p) const { return Pts<T>(x + p.x, y + p.y); }
		Pts<T> operator + (const T& i) const { return Pts<T>(x + i, y + i); }
		Pts<T> operator * (const Pts<T>& p) const { return Pts<T>(x * p.x, y * p.y); }
		Pts<T> operator * (const T& i) const { return Pts<T>(x * i, y * i); }
		Pts<T> operator / (const Pts<T>& p) const { return Pts<T>(x / p.x, y / p.y); }
		Pts<T> operator / (const T& i) const { return Pts<T>(x / i, y / i); }
		const Pts<T>& operator -= (const Pts<T>& p) { x -= p.x; y -= p.y; return *this; }
		const Pts<T>& operator -= (const T& i) { x -= i; y -= i; return *this; }
		const Pts<T>& operator += (const Pts<T>& p) { x += p.x; y += p.y; return *this; }
		const Pts<T>& operator += (const T& i) { x += i; y += i; return *this; }
		const Pts<T>& operator /= (const Pts<T>& p) { x /= p.x; y /= p.y; return *this; }
		const Pts<T>& operator /= (const T& i) { x /= i; y /= i; return *this; }
		const Pts<T>& operator *= (const Pts<T>& p) { x *= p.x; y *= p.y; return *this; }
		const Pts<T>& operator *= (const T& i) { x *= i; y *= i; return *this; }
		// ������������
		const Pts<T>& operator = (const Bar<T>& r) { x = r.x; y = r.y; return *this; }
		// �������
		friend Pts<T> operator - (const T& i, const Pts<T>& p) { return Pts<T>(i - p.x, i - p.y); }
		friend Pts<T> operator + (const T& i, const Pts<T>& p) { return Pts<T>(i + p.x, i + p.y); }
		friend Pts<T> operator / (const T& i, const Pts<T>& p) { return Pts<T>(i / p.x, i / p.y); }
		friend Pts<T> operator * (const T& i, const Pts<T>& p) { return Pts<T>(i * p.x, i * p.y); }
		// ���������
		bool operator == (const Pts<T>& p) const { return (x == p.x && y == p.y); }
		bool operator != (const Pts<T>& p) const { return (x != p.x || y != p.y); }
		bool operator == (const Bar<T>& r) const { return (x == r.x && y == r.y); }
		bool operator != (const Bar<T>& r) const { return (x != r.x || y != r.y); }
		bool is_empty() const { return (x == 0 && y == 0); }
		// ���������� �����
		operator T*() const { return (T*)&x; }
		operator POINT*() const { return (POINT*)&x; }
		operator POINT() const { return (POINT&)x; }
		operator const POINT*() const { return (const POINT*)&x; }
		// �����������
		const Pts<T>& empty() { x = y = 0; return *this; }
		const Pts<T>& set(const T& X, const T& Y) { x = X; y = Y; return *this; }
		T x, y;
	};

	template <typename T = ssh_u> class Range
	{
	public:
		// ������������
		Range() : w(0), h(0) {}
		Range(const T* ptr) { w = ptr[0]; h = ptr[1]; }
		Range(const T& W, const T& H) : w(W), h(H) {}
		Range(const Range<T>& s) : w(s.w), h(s.h) {}
		Range(const SIZE& s) : w(s.cx), h(s.cy) {}
		Range(const Bar<T>& r) : w(r.w), h(r.h) {}
		// ��������
		Range<T> operator - (const Range<T>& s) const { return Range<T>(w - s.w, h - s.h); }
		Range<T> operator - (const T& i) const { return Range<T>(w - i, h - i); }
		Range<T> operator + (const Range<T>& s) const { return Range<T>(w + s.w, h + s.h); }
		Range<T> operator + (const T& i) const { return Range<T>(w + i, h + i); }
		Range<T> operator * (const Range<T>& s) const { return Range<T>(w * s.w, h * s.h); }
		Range<T> operator * (const T& i) const { return Range<T>(w * i, h * i); }
		Range<T> operator / (const Range<T>& s) const { return Range<T>(w / s.w, h / s.h); }
		Range<T> operator / (const T& i) const { return Range<T>(w / i, h / i); }
		const Range<T>& operator += (const Range<T>& s) { w += s.w; h += s.h; return *this; }
		const Range<T>& operator += (const T& s) { w += s; h += s; return *this; }
		const Range<T>& operator -= (const Range<T>& s) { w -= s.w; h -= s.h; return *this; }
		const Range<T>& operator -= (const T& s) { w -= s; h -= s; return *this; }
		const Range<T>& operator /= (const Range<T>& s) { w /= s.w; h /= s.h; return *this; }
		const Range<T>& operator /= (const T& s) { w /= s; h /= s; return *this; }
		const Range<T>& operator *= (const Range<T>& s) { w *= s.w; h *= s.h; return *this; }
		const Range<T>& operator *= (const T& s) { w *= s; h *= s; return *this; }
		// ������������
		const Range<T>& operator = (const Bar<T>& r) { w = r.w; h = r.h; return *this; }
		// �������
		friend Range<T> operator - (const T& i, const Range<T>& p) { return Range<T>(i - p.w, i - p.h); }
		friend Range<T> operator + (const T& i, const Range<T>& p) { return Range<T>(i + p.w, i + p.h); }
		friend Range<T> operator * (const T& i, const Range<T>& p) { return Range<T>(i * p.w, i * p.h); }
		friend Range<T> operator / (const T& i, const Range<T>& p) { return Range<T>(i / p.w, i / p.h); }
		// ��������
		bool operator == (const Range<T>& s) const { return (w == s.w && h == s.h); }
		bool operator == (const Bar<T>& r) const { return (w == r.w && h == r.h); }
		bool operator != (const Range<T>& s) const { return (w != s.w || h != s.h); }
		bool operator != (const Bar<T>& r) const { return (w != r.w || h != r.h); }
		// ���������� �����
		operator T*() const { return (T*)&w; }
		operator SIZE*() const { return (SIZE*)&w; }
		operator SIZE() const { return (SIZE&)w; }
		operator const SIZE*() const { return (const SIZE*)&w; }
		// ��������
		bool is_empty() const { return (w == 0 && h == 0); }
		const Range<T>& empty() { w = h = 0; return *this; }
		const Range<T>& set(const T& W, const T& H) { w = W; h = H; return *this; }
		T w, h;
	};

	template <typename T = ssh_u> class Bar
	{
	public:
		// ������������
		Bar() : x(0), y(0), w(0), h(0) {}
		Bar(const T* ptr) { x = ptr[0]; y = ptr[1]; w = ptr[2]; h = ptr[3]; }
		Bar(const T& X, const T& Y, const T& W, const T& H) : x(X), y(Y), w(W), h(H) {}
		Bar(const RECT& r) : x(r.left), y(r.top), w(r.right - x), h(r.bottom - y) {}
		Bar(const Bar<T>& r) : x(r.x), y(r.y), w(r.w), h(r.h) {}
		Bar(const Pts<T>& pt, const Range<T>& s) : x(pt.x), y(pt.y), w(s.w), h(s.h) {}
		Bar(const Range<T>& sz) : x(0), y(0), w(sz.w), h(sz.h) {}
		Bar(const Pts<T>& pt) : x(pt.x), y(pt.y), w(0), h(0) {}
		// ��������
		Bar<T> operator - (const Bar<T>& r) const { return Bar<T>(x - r.x, y - r.y, w - r.w, h - r.h); }
		Bar<T> operator - (const T& i) const { return Bar<T>(x - i, y - i, w - i, h - i); }
		Bar<T> operator - (const Pts<T>& p) const { return Bar<T>(x - p.x, y - p.y, w - p.x, h - p.y); }
		Bar<T> operator - (const Range<T>& s) const { return Bar<T>(x - s.w, y - s.h, w - s.w, h - s.h); }
		Bar<T> operator + (const Bar<T>& r) const { return Bar<T>(x + r.x, y + r.y, w + r.w, h + r.h); }
		Bar<T> operator + (const Pts<T>& p) const { return Bar<T>(x + p.x, y + p.y, w + p.x, h + p.y); }
		Bar<T> operator + (const T& i) const { return Bar<T>(x + i, y + i, w + i, h + i); }
		Bar<T> operator + (const Range<T>& s) const { return Bar<T>(x + s.w, y + s.h, w + s.w, h + s.h); }
		Bar<T> operator / (const Bar<T>& r) const { return Bar<T>(x / r.x, y / r.y, w / r.w, h / r.h); }
		Bar<T> operator / (const T& i) const { return Bar<T>(x / i, y / i, w / i, h / i); }
		Bar<T> operator / (const Pts<T>& p) const { return Bar<T>(x / p.x, y / p.y, w / p.x, h / p.y); }
		Bar<T> operator / (const Range<T>& s) const { return Bar<T>(x / s.w, y / s.h, w / s.w, h / s.h); }
		Bar<T> operator * (const Bar<T>& r) const { return Bar<T>(x * r.x, y * r.y, w * r.w, h * r.h); }
		Bar<T> operator * (const T& i) const { return Bar<T>(x * i, y * i, w * i, h * i); }
		Bar<T> operator * (const Pts<T>& p) const { return Bar<T>(x * p.x, y * p.y, w * p.x, h * p.y); }
		Bar<T> operator * (const Range<T>& s) const { return Bar<T>(x * s.w, y * s.h, w * s.w, h * s.h); }
		const Bar<T>& operator -= (const Bar<T>& r) { x -= r.x; y -= r.y; w -= r.w; h -= r.h; return *this; }
		const Bar<T>& operator -= (const Pts<T>& p) { x -= p.x; y -= p.y; w -= p.x; h -= p.y; return *this; }
		const Bar<T>& operator -= (const T& i) { x -= i; y -= i; w -= i; h -= i; return *this; }
		const Bar<T>& operator -= (const Range<T>& s) { x -= s.w; y -= s.h; w -= s.w; h -= s.h; return *this; }
		const Bar<T>& operator += (const Bar<T>& r) { x += r.x; y += r.y; w += r.w; h += r.h; return *this; }
		const Bar<T>& operator += (const Pts<T>& p) { x += p.x; y += p.y; w += p.x; h += p.y; return *this; }
		const Bar<T>& operator += (const T& i) { x += i; y += i; w += i; h += i; return *this; }
		const Bar<T>& operator /= (const Bar<T>& r) { x /= r.x; y /= r.y; w /= r.w; h /= r.h; return *this; }
		const Bar<T>& operator /= (const T& i) { x /= i; y /= i; w /= i; h /= i; return *this; }
		const Bar<T>& operator /= (const Pts<T>& p) { x /= p.x; y /= p.y; w / p.x; h /= p.y; return *this; }
		const Bar<T>& operator /= (const Range<T>& s) { x /= s.w; y /= s.h; w /= s.w; h /= s.h; return *this; }
		const Bar<T>& operator *= (const Bar<T>& r) { x *= r.x; y *= r.y; w *= r.w; h *= r.h; return *this; }
		const Bar<T>& operator *= (const T& i) { x *= i; y *= i; w *= i; h *= i; return *this; }
		const Bar<T>& operator *= (const Pts<T>& p) { x *= p.x; y *= p.y; w *= p.x; h *= p.y; return *this; }
		const Bar<T>& operator *= (const Range<T>& s) { x *= s.w; y *= s.h; w *= s.w; h *= s.h; return *this; }
		// �������
		friend Bar<T> operator - (const Pts<T>& p, const Bar<T>& r) { return Bar<T>(p.x - r.x, p.y - r.y, p.x - r.w, p.y - r.h); }
		friend Bar<T> operator - (const T& i, const Bar<T>& r) { return Bar<T>(i - r.x, i - r.y, i - r.w, i - r.h); }
		friend Bar<T> operator - (const Range<T>& s, const Bar<T>& r) { return Bar<T>(s.w - r.x, s.h - r.y, s.w - r.w, s.h - r.h); }
		friend Bar<T> operator + (const Pts<T>& p, const Bar<T>& r) { return Bar<T>(p.x + r.x, p.y + r.y, p.x + r.w, p.y + r.h); }
		friend Bar<T> operator + (const T& i, const Bar<T>& r) { return Bar<T>(i + r.x, i + r.y, i + r.w, i + r.h); }
		friend Bar<T> operator + (const Range<T>& s, const Bar<T>& r) { return Bar<T>(s.w + r.x, s.h + r.y, s.w + r.w, s.h + r.h); }
		friend Bar<T> operator / (const Pts<T>& p, const Bar<T>& r) { return Bar<T>(p.x / r.x, p.y / r.y, p.x / r.w, p.y / r.h); }
		friend Bar<T> operator / (const T& i, const Bar<T>& r) { return Bar<T>(i / r.x, i / r.y, i / r.w, i / r.h); }
		friend Bar<T> operator / (const Range<T>& s, const Bar<T>& r) { return Bar<T>(s.w / r.x, s.h / r.y, s.w / r.w, s.h / r.h); }
		friend Bar<T> operator * (const Pts<T>& p, const Bar<T>& r) { return Bar<T>(p.x * r.x, p.y * r.y, p.x * r.w, p.y * r.h); }
		friend Bar<T> operator * (const T& i, const Bar<T>& r) { return Bar<T>(i * r.x, i * r.y, i * r.w, i * r.h); }
		friend Bar<T> operator * (const Range<T>& s, const Bar<T>& r) { return Bar<T>(s.w * r.x, s.h * r.y, s.w * r.w, s.h * r.h); }
		// ���������
		bool operator == (const Pts<T>& p) const { return (x == p.x && y == p.y); }
		bool operator == (const Bar<T>& r) const { return (x == r.x && y == r.y && w == r.w && h == r.h); }
		bool operator == (const Range<T>& s) const { return (w == s.w && h == s.h); }
		bool operator != (const Pts<T>& p) const { return (x != p.x || y != p.y); }
		bool operator != (const Bar<T>& r) const { return (x != r.x || y != r.y || w != r.w || h != r.h); }
		bool operator != (const Range<T>& s) const { return (w != s.w || h != s.h); }
		// ������������
		const Bar<T>& operator = (const Bar<T>& r) { x = r.x; y = r.y; w = r.w; h = r.h; return *this; }
		const Bar<T>& operator = (const RECT& r) { x = r.left; y = r.top; w = r.right - x; h = r.bottom - y; return *this; }
		const Bar<T>& operator = (const Range<T>& s) { w = s.w; h = s.h; return *this; }
		const Bar<T>& operator = (const Pts<T>& p) { x = p.x; y = p.y; return *this; }
		// ���������� �����
		operator const T*() const { return (const T*)&x; }
		operator T*() { return (T*)&x; }
		operator RECT*() const { static RECT r; r.left = x; r.top = y; r.bottom = y + h; r.right = x + w; return &r; }
		operator const RECT*() const { static RECT r; r.left = x; r.top = y; r.bottom = y + h; r.right = x + w; return &r; }
		operator Range<T>() const { return Range<T>(w, h); }
		operator Pts<T>() const { return Pts<T>(x, y); }
		// �����������
		const Bar<T>& set(const T& X, const T& Y, const T& W, const T& H) { x = X; y = Y; w = W; h = H; return *this; }
		const Bar<T>& set(const Pts<T>& pt, const Range<T>& sz) { x = pt.x; y = pt.y; w = sz.w; h = sz.h; return *this; }
		const Bar<T>& null() { x = y = w = h = 0; return *this; }
		const Bar<T>& empty() { w = h = 0; return *this; }
		Pts<T> center() const { return Pts<T>(x + (w / 2), y + (h / 2)); }
		bool is_null() const { return (x == 0 && y == 0 && (w <= 0 || h <= 0)); }
		bool is_empty() const { return (w <= 0 || h <= 0); }
		bool ptInRc(const Pts<T>& pt) const { return ((pt.x >= x && pt.x < (x + w)) && (pt.y >= y && pt.y < (y + h))); }
		bool rcInRc(const Bar<T>& rc, RECT* dst) const
		{
			RECT r1, r2;
			::SetRect(&r1, x, y, x + w, y + h);
			::SetRect(&r2, rc.x, rc.y, rc.x + rc.w, rc.y + rc.h);
			return (::IntersectRect(dst, &r1, &r2) != 0);
		}
		const Bar<T>& inflate(const T& X, const T& Y, const T& W, const T& H) { x += X; y += Y; w += W; h += H; return *this; }
		const T right() const { return (x + w); }
		const T bottom() const { return (y + h); }
		union
		{
			struct
			{
				Pts<T> point;
				Range<T> range;
			};
			struct
			{
				T x;
				T y;
				T w;
				T h;
			};
		};
	};

	template <typename T1 = long, typename T2 = float> class Box
	{
	public:
		// ������������
		Box<T1, T2>() : x(0), y(0), w(0), h(0), n(0), f(0) {}
		Box<T1, T2>(const Bar<T1>& r, const Range<T2>& s) : x(r.x), y(r.y), w(r.w), h(r.h), n(s.w), f(s.h) {}
		Box<T1, T2>(const T1& X, const T1& Y, const T1& W, const T1& H, const T2& N, const T2& F) : x(X), y(Y), w(W), h(H), n(N), f(F) {}
		Box<T1, T2>(const Box<T1, T2>& b) : x(b.x), y(b.y), w(b.w), h(b.h), n(b.n), f(b.f) {}
		// ���������
		// ����������
		bool operator == (const Box<T1, T2>& b) const { return (x == b.x && y == b.y && w == b.w && h == b.h && n == b.n && f == b.f); }
		bool operator != (const Box<T1, T2>& b) const { return (x != b.x || y != b.y || w != b.w || h != b.h || n != b.n || f != b.f); }
		// ������������
		const Box<T1, T2>& operator = (const Box<T1, T2>& b) { SSH_MEMCPY(*this, &b, sizeof(Box<T1, T2>)); return *this; }
		const Box<T1, T2>& operator = (const Bar<T1>& r) { x = r.x; y = r.y; w = r.w; h = r.h; return *this; }
		const Box<T1, T2>& operator = (const Range<T2>& s) { n = s.w; f = s.h; return *this; }
		// ��������������
		Box<T1, T2> operator + (const Bar<T1>& r) const { return Box<T1, T2>(x + r.x, y + r.y, w + r.w, h + r.h, n, f); }
		Box<T1, T2> operator + (const Range<T2>& s) const { return Box<T1, T2>(x, y, w, h, n + s.w, f + s.h); }
		Box<T1, T2> operator - (const Bar<T1>& r) const { return Box<T1, T2>(x - r.x, y - r.y, w - r.w, h - r.h, n, f); }
		Box<T1, T2> operator - (const Range<T2>& s) const { return Box<T1, T2>(x, y, w, h, n - s.w, f - s.h); }
		const Box<T1, T2>& operator += (const Bar<T1>& r) { x += r.x; y += r.y; w += r.w; h += r.h; return this; }
		const Box<T1, T2>& operator += (const Range<T2>& s) { n += s.w; f += s.h; return this; }
		const Box<T1, T2>& operator -= (const Bar<T1>& r) { x -= r.x; y -= r.y; w -= r.w; h -= r.h; return this; }
		const Box<T1, T2>& operator -= (const Range<T2>& s) { n -= s.w; f -= s.h; return this; }
		// ���������� ����
		operator Bar<T1>*() const { return Bar<T1>(x, y, w, h); }
		operator const Bar<T1>*() const { return Bar<T1>(x, y, w, h); }
		operator Range<T2>*() const { return Range<T2>(n, f); }
		operator const Range<T2>*() const { return Range<T2>(n, f); }
		// �����������
		bool is_null() const { return (x == 0 && y == 0 && w == 0 && h == 0 && n == 0 && f == 0); }
		bool is_empty() const { return (w == 0 && h == 0 && n == 0 && f == 0); }
		T1 x, y, w, h;
		T2 n, f;
	};

	template<ssh_u i> class HashCompiletime
	{
	public:
		static __forceinline ssh_u make(const ssh_u hash, const ssh_u len, ssh_cws str)
		{
			return HashCompiletime<i - 1>::make(hash ^ ((hash << 5) + str[len - i] + (hash >> 2)), len, str);
		}
	};

	template <> class HashCompiletime<0>
	{
	public:
		static __forceinline ssh_u make(const ssh_u hash, const ssh_u len, ssh_cws str)
		{
			return hash ^ ((hash << 5) + str[len] + (hash >> 2));
		}
	};

	template<ssh_u N> __forceinline ssh_u HashCT(const wchar_t(&str)[N])
	{
		return HashCompiletime<N - 2>::make(1315423911, N - 2, str);
	}
}
