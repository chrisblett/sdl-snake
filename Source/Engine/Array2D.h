#pragma once

#include <algorithm>
#include <cassert>
#include <cstring>

template <class T>
class Array2D
{
public:
	Array2D(int width = 0, int height = 0)
		: m_array(width* height ? new T[width * height] : nullptr)
		, m_width(width)
		, m_height(height)
	{
	}

	Array2D(const Array2D& src)
		: m_array(src.m_array ? new T[src.m_width * src.m_height] : nullptr)
		, m_width(src.m_width)
		, m_height(src.m_height)
	{
		std::copy(src.m_array, src.m_array + src.Size(), m_array);
	}

	~Array2D()
	{
		if (m_array != 0)
		{
			delete[] m_array;
		}
		m_array = 0;
	}

	Array2D& operator=(Array2D rhs)
	{
		Swap(*this, rhs);
		return *this;
	}

	friend void Swap(Array2D& a, Array2D& b)
	{
		std::swap(a.m_width, b.m_width);
		std::swap(a.m_height, b.m_height);
		std::swap(a.m_array, b.m_array);
	}

	void Resize(int newWidth, int newHeight)
	{
		T* newArray = new T[newWidth * newHeight];

		// Determine the size of the sub-array that can fit within the new array
		int minX = (newWidth < m_width ? newWidth : m_width);
		int minY = (newHeight < m_height ? newHeight : m_height);

		// Move data over
		for (int y = 0; y < minY; y++)
		{
			int newRowStartIndex = y * newWidth;
			int oldRowStartIndex = y * m_width;

			for (int x = 0; x < minX; x++)
			{
				// Calculate the index of row and col in new array and copy from index in old array
				newArray[newRowStartIndex + x] = m_array[oldRowStartIndex + x];
			}
		}

		if (m_array != 0)
		{
			delete[] m_array;
		}

		m_array = newArray;
		m_width = newWidth;
		m_height = newHeight;
	}

	T& Get(int x, int y)
	{
		assert(x >= 0 && x < m_width);
		assert(y >= 0 && y < m_height);

		return m_array[y * m_width + x];
	}

	const T& Get(int x, int y) const
	{
		assert(x >= 0 && x < m_width);
		assert(y >= 0 && y < m_height);

		return m_array[y * m_width + x];
	}

	int Width()  const { return m_width; }
	int Height() const { return m_height; }
	int Size()   const { return m_width * m_height; }

private:
	T* m_array;
	int m_width;
	int m_height;
};