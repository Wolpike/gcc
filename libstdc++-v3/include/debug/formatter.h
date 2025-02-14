// Debug-mode error formatting implementation -*- C++ -*-

// Copyright (C) 2003-2015 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file debug/formatter.h
 *  This file is a GNU debug extension to the Standard C++ Library.
 */

#ifndef _GLIBCXX_DEBUG_FORMATTER_H
#define _GLIBCXX_DEBUG_FORMATTER_H 1

#include <bits/c++config.h>
#include <bits/cpp_type_traits.h>

#if __cpp_rtti
# include <typeinfo>
# define _GLIBCXX_TYPEID(_Type) &typeid(_Type)
#else
namespace std
{
  class type_info;
}
# define _GLIBCXX_TYPEID(_Type) 0
#endif

namespace __gnu_debug
{
  using std::type_info;

  template<typename _Iterator>
    bool __check_singular(const _Iterator&);

  class _Safe_sequence_base;

  template<typename _Iterator, typename _Sequence>
    class _Safe_iterator;

  template<typename _Iterator, typename _Sequence>
    class _Safe_local_iterator;

  template<typename _Sequence>
    class _Safe_sequence;

  enum _Debug_msg_id
  {
    // General checks
    __msg_valid_range,
    __msg_insert_singular,
    __msg_insert_different,
    __msg_erase_bad,
    __msg_erase_different,
    __msg_subscript_oob,
    __msg_empty,
    __msg_unpartitioned,
    __msg_unpartitioned_pred,
    __msg_unsorted,
    __msg_unsorted_pred,
    __msg_not_heap,
    __msg_not_heap_pred,
    // std::bitset checks
    __msg_bad_bitset_write,
    __msg_bad_bitset_read,
    __msg_bad_bitset_flip,
    // std::list checks
    __msg_self_splice,
    __msg_splice_alloc,
    __msg_splice_bad,
    __msg_splice_other,
    __msg_splice_overlap,
    // iterator checks
    __msg_init_singular,
    __msg_init_copy_singular,
    __msg_init_const_singular,
    __msg_copy_singular,
    __msg_bad_deref,
    __msg_bad_inc,
    __msg_bad_dec,
    __msg_iter_subscript_oob,
    __msg_advance_oob,
    __msg_retreat_oob,
    __msg_iter_compare_bad,
    __msg_compare_different,
    __msg_iter_order_bad,
    __msg_order_different,
    __msg_distance_bad,
    __msg_distance_different,
    // istream_iterator
    __msg_deref_istream,
    __msg_inc_istream,
    // ostream_iterator
    __msg_output_ostream,
    // istreambuf_iterator
    __msg_deref_istreambuf,
    __msg_inc_istreambuf,
    // forward_list
    __msg_insert_after_end,
    __msg_erase_after_bad,
    __msg_valid_range2,
    // unordered container local iterators
    __msg_local_iter_compare_bad,
    __msg_non_empty_range,
    // self move assign
    __msg_self_move_assign,
    // unordered container buckets
    __msg_bucket_index_oob,
    __msg_valid_load_factor,
    // others
    __msg_equal_allocs,
    __msg_insert_range_from_self
  };

  class _Error_formatter
  {
    /// Whether an iterator is constant, mutable, or unknown
    enum _Constness
    {
      __unknown_constness,
      __const_iterator,
      __mutable_iterator,
      __last_constness
    };

    // The state of the iterator (fine-grained), if we know it.
    enum _Iterator_state
    {
      __unknown_state,
      __singular,	// singular, may still be attached to a sequence
      __begin,		// dereferenceable, and at the beginning
      __middle,		// dereferenceable, not at the beginning
      __end,		// past-the-end, may be at beginning if sequence empty
      __before_begin,	// before begin
      __last_state
    };

    // Tags denoting the type of parameter for construction
    struct _Is_iterator { };
    struct _Is_iterator_value_type { };
    struct _Is_sequence { };
    struct _Is_instance { };

  public:
    // A parameter that may be referenced by an error message
    struct _Parameter
    {
      enum
      {
	__unused_param,
	__iterator,
	__sequence,
	__integer,
	__string,
	__instance,
	__iterator_value_type
      } _M_kind;

      struct _Type
      {
	const char*		_M_name;
	const type_info*	_M_type;
      };

      struct _Instance : _Type
      {
	const void*		_M_address;
      };

      union
      {
	// When _M_kind == __iterator
	struct : _Instance
	{
	  _Constness		_M_constness;
	  _Iterator_state	_M_state;
	  const void*		_M_sequence;
	  const type_info*	_M_seq_type;
	} _M_iterator;

	// When _M_kind == __sequence
	_Instance _M_sequence;

	// When _M_kind == __integer
	struct
	{
	  const char*		_M_name;
	  long			_M_value;
	} _M_integer;

	// When _M_kind == __string
	struct
	{
	  const char*		_M_name;
	  const char*		_M_value;
	} _M_string;

	// When _M_kind == __instance
	_Instance _M_instance;

	// When _M_kind == __iterator_value_type
	_Type _M_iterator_value_type;
      } _M_variant;

      _Parameter() : _M_kind(__unused_param), _M_variant() { }

      _Parameter(long __value, const char* __name)
      : _M_kind(__integer), _M_variant()
      {
	_M_variant._M_integer._M_name = __name;
	_M_variant._M_integer._M_value = __value;
      }

      _Parameter(const char* __value, const char* __name)
      : _M_kind(__string), _M_variant()
      {
	_M_variant._M_string._M_name = __name;
	_M_variant._M_string._M_value = __value;
      }

      template<typename _Iterator, typename _Sequence>
	_Parameter(_Safe_iterator<_Iterator, _Sequence> const& __it,
		   const char* __name, _Is_iterator)
	: _M_kind(__iterator),  _M_variant()
	{
	  _M_variant._M_iterator._M_name = __name;
	  _M_variant._M_iterator._M_address = &__it;
	  _M_variant._M_iterator._M_type = _GLIBCXX_TYPEID(__it);
	  _M_variant._M_iterator._M_constness =
	    std::__are_same<_Safe_iterator<_Iterator, _Sequence>,
			    typename _Sequence::iterator>::
	      __value ? __mutable_iterator : __const_iterator;
	  _M_variant._M_iterator._M_sequence = __it._M_get_sequence();
	  _M_variant._M_iterator._M_seq_type = _GLIBCXX_TYPEID(_Sequence);

	  if (__it._M_singular())
	    _M_variant._M_iterator._M_state = __singular;
	  else
	    {
	      if (__it._M_is_before_begin())
		_M_variant._M_iterator._M_state = __before_begin;
	      else if (__it._M_is_end())
		_M_variant._M_iterator._M_state = __end;
	      else if (__it._M_is_begin())
		_M_variant._M_iterator._M_state = __begin;
	      else
		_M_variant._M_iterator._M_state = __middle;
	    }
	}

      template<typename _Iterator, typename _Sequence>
	_Parameter(_Safe_local_iterator<_Iterator, _Sequence> const& __it,
		   const char* __name, _Is_iterator)
	: _M_kind(__iterator),  _M_variant()
	{
	  _M_variant._M_iterator._M_name = __name;
	  _M_variant._M_iterator._M_address = &__it;
	  _M_variant._M_iterator._M_type = _GLIBCXX_TYPEID(__it);
	  _M_variant._M_iterator._M_constness =
	    std::__are_same<_Safe_local_iterator<_Iterator, _Sequence>,
			    typename _Sequence::local_iterator>::
	      __value ? __mutable_iterator : __const_iterator;
	  _M_variant._M_iterator._M_sequence = __it._M_get_sequence();
	  _M_variant._M_iterator._M_seq_type = _GLIBCXX_TYPEID(_Sequence);

	  if (__it._M_singular())
	    _M_variant._M_iterator._M_state = __singular;
	  else
	    {
	      if (__it._M_is_end())
		_M_variant._M_iterator._M_state = __end;
	      else if (__it._M_is_begin())
		_M_variant._M_iterator._M_state = __begin;
	      else
		_M_variant._M_iterator._M_state = __middle;
	    }
	}

      template<typename _Type>
	_Parameter(const _Type* const& __it, const char* __name, _Is_iterator)
	: _M_kind(__iterator), _M_variant()
	{
	  _M_variant._M_iterator._M_name = __name;
	  _M_variant._M_iterator._M_address = &__it;
	  _M_variant._M_iterator._M_type = _GLIBCXX_TYPEID(__it);
	  _M_variant._M_iterator._M_constness = __const_iterator;
	  _M_variant._M_iterator._M_state = __it ? __unknown_state : __singular;
	  _M_variant._M_iterator._M_sequence = 0;
	  _M_variant._M_iterator._M_seq_type = 0;
	}

      template<typename _Type>
	_Parameter(_Type* const& __it, const char* __name, _Is_iterator)
	: _M_kind(__iterator), _M_variant()
	{
	  _M_variant._M_iterator._M_name = __name;
	  _M_variant._M_iterator._M_address = &__it;
	  _M_variant._M_iterator._M_type = _GLIBCXX_TYPEID(__it);
	  _M_variant._M_iterator._M_constness = __mutable_iterator;
	  _M_variant._M_iterator._M_state = __it ? __unknown_state : __singular;
	  _M_variant._M_iterator._M_sequence = 0;
	  _M_variant._M_iterator._M_seq_type = 0;
	}

      template<typename _Iterator>
	_Parameter(_Iterator const& __it, const char* __name, _Is_iterator)
	: _M_kind(__iterator), _M_variant()
	{
	  _M_variant._M_iterator._M_name = __name;
	  _M_variant._M_iterator._M_address = &__it;
	  _M_variant._M_iterator._M_type = _GLIBCXX_TYPEID(__it);
	  _M_variant._M_iterator._M_constness = __unknown_constness;
	  _M_variant._M_iterator._M_state =
	    __gnu_debug::__check_singular(__it) ? __singular : __unknown_state;
	  _M_variant._M_iterator._M_sequence = 0;
	  _M_variant._M_iterator._M_seq_type = 0;
	}

      template<typename _Sequence>
	_Parameter(const _Safe_sequence<_Sequence>& __seq,
		   const char* __name, _Is_sequence)
	: _M_kind(__sequence), _M_variant()
	{
	  _M_variant._M_sequence._M_name = __name;
	  _M_variant._M_sequence._M_address =
	    static_cast<const _Sequence*>(&__seq);
	  _M_variant._M_sequence._M_type = _GLIBCXX_TYPEID(_Sequence);
	}

      template<typename _Sequence>
	_Parameter(const _Sequence& __seq, const char* __name, _Is_sequence)
	: _M_kind(__sequence), _M_variant()
	{
	  _M_variant._M_sequence._M_name = __name;
	  _M_variant._M_sequence._M_address = &__seq;
	  _M_variant._M_sequence._M_type = _GLIBCXX_TYPEID(_Sequence);
	}

      template<typename _Iterator>
	_Parameter(const _Iterator& __it, const char* __name,
		   _Is_iterator_value_type)
	: _M_kind(__iterator_value_type), _M_variant()
	{
	  _M_variant._M_iterator_value_type._M_name = __name;
	  _M_variant._M_iterator_value_type._M_type =
	    _GLIBCXX_TYPEID(typename std::iterator_traits<_Iterator>::value_type);
	}

      template<typename _Type>
	_Parameter(const _Type& __inst, const char* __name, _Is_instance)
	: _M_kind(__instance), _M_variant()
	{
	  _M_variant._M_instance._M_name = __name;
	  _M_variant._M_instance._M_address = &__inst;
	  _M_variant._M_instance._M_type = _GLIBCXX_TYPEID(_Type);
	}

      void
      _M_print_field(const _Error_formatter* __formatter,
		     const char* __name) const;

      void
      _M_print_description(const _Error_formatter* __formatter) const;
    };

    template<typename _Iterator>
      const _Error_formatter&
      _M_iterator(const _Iterator& __it, const char* __name = 0)  const
      {
	if (_M_num_parameters < std::size_t(__max_parameters))
	  _M_parameters[_M_num_parameters++] = _Parameter(__it, __name,
							  _Is_iterator());
	return *this;
      }

    template<typename _Iterator>
      const _Error_formatter&
      _M_iterator_value_type(const _Iterator& __it,
			     const char* __name = 0)  const
      {
	if (_M_num_parameters < std::size_t(__max_parameters))
	  _M_parameters[_M_num_parameters++] =
	    _Parameter(__it, __name, _Is_iterator_value_type());
	return *this;
      }

    const _Error_formatter&
    _M_integer(long __value, const char* __name = 0) const
    {
      if (_M_num_parameters < std::size_t(__max_parameters))
	_M_parameters[_M_num_parameters++] = _Parameter(__value, __name);
      return *this;
    }

    const _Error_formatter&
    _M_string(const char* __value, const char* __name = 0) const
    {
      if (_M_num_parameters < std::size_t(__max_parameters))
	_M_parameters[_M_num_parameters++] = _Parameter(__value, __name);
      return *this;
    }

    template<typename _Sequence>
      const _Error_formatter&
      _M_sequence(const _Sequence& __seq, const char* __name = 0) const
      {
	if (_M_num_parameters < std::size_t(__max_parameters))
	  _M_parameters[_M_num_parameters++] = _Parameter(__seq, __name,
							  _Is_sequence());
	return *this;
      }

    template<typename _Type>
      const _Error_formatter&
      _M_instance(const _Type& __inst, const char* __name = 0) const
      {
	if (_M_num_parameters < std::size_t(__max_parameters))
	  _M_parameters[_M_num_parameters++] = _Parameter(__inst, __name,
							  _Is_instance());
	return *this;
      }

    const _Error_formatter&
    _M_message(const char* __text) const
    { _M_text = __text; return *this; }

    const _Error_formatter&
    _M_message(_Debug_msg_id __id) const throw ();

    _GLIBCXX_NORETURN void
    _M_error() const;

    template<typename _Tp>
      void
      _M_format_word(char*, int, const char*, _Tp) const throw ();

    void
    _M_print_word(const char* __word) const;

    void
    _M_print_string(const char* __string) const;

  private:
    _Error_formatter(const char* __file, std::size_t __line)
    : _M_file(__file), _M_line(__line), _M_num_parameters(0), _M_text(0),
      _M_max_length(78), _M_column(1), _M_first_line(true), _M_wordwrap(false)
    { _M_get_max_length(); }

    void
    _M_get_max_length() const throw ();

    enum { __max_parameters = 9 };

    const char*		_M_file;
    std::size_t		_M_line;
    mutable _Parameter	_M_parameters[__max_parameters];
    mutable std::size_t	_M_num_parameters;
    mutable const char*	_M_text;
    mutable std::size_t	_M_max_length;
    enum { _M_indent = 4 } ;
    mutable std::size_t	_M_column;
    mutable bool	_M_first_line;
    mutable bool	_M_wordwrap;

  public:
    static _Error_formatter
    _M_at(const char* __file, std::size_t __line)
    { return _Error_formatter(__file, __line); }
  };
} // namespace __gnu_debug

#undef _GLIBCXX_TYPEID

#endif
