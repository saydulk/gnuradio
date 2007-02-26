/* -*- c++ -*- */
/*
 * Copyright 2006,2007 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_MB_MBLOCK_IMPL_H
#define INCLUDED_MB_MBLOCK_IMPL_H

#include <mb_mblock.h>
#include <mb_connection.h>
#include <mb_msg_queue.h>
#include <list>
#include <map>


typedef std::map<std::string, mb_port_sptr>   mb_port_map_t;
typedef std::map<std::string, mb_mblock_sptr> mb_comp_map_t;


/*!
 * \brief The private implementation details of the mblock system.
 */
class mb_mblock_impl : boost::noncopyable
{
  mb_mblock		       *d_mb;		// pointer to our associated mblock
  mb_mblock		       *d_mb_parent;	// pointer to our parent

  std::string			d_fullname;	// hierarchical name

  mb_port_map_t			d_port_map;	// our ports
  mb_comp_map_t			d_comp_map;	// our components
  mb_conn_table			d_conn_table;	// our connections

  mb_msg_queue			d_msgq;		// incoming messages for us

public:
  mb_mblock_impl(mb_mblock *mb);
  ~mb_mblock_impl();

  /*!
   * \brief Define a port.
   *
   * EXTERNAL and RELAY ports are part of our peer interface.
   * INTERNAL ports are used to talk to sub-components.
   *
   * \param port_name    The name of the port (must be unique within this mblock).
   * \param protocol_class_name	The name of the protocol class associated with
   *				this port.  It must already be defined.
   * \param conjugated   Are the incoming and outgoing message sets swapped?
   * \param port_type    INTERNAL, EXTERNAL or RELAY.
   */
  mb_port_sptr
  define_port(const std::string &port_name,
	      const std::string &protocol_class_name,
	      bool conjugated,
	      mb_port::port_type_t port_type);

  /*!
   * \brief Define a subcomponent by name.
   *
   * Called within the constructor to tell the system the
   * names and identities of our sub-component mblocks.
   *
   * \param component_name  The name of the sub-component (must be unique with this mblock).
   * \param component       The sub-component instance.
   */
  void
  define_component(const std::string &component_name,
		   mb_mblock_sptr component);

  /*!
   * \brief connect endpoint_1 to endpoint_2
   *
   * \param comp_name1  component on one end of the connection
   * \param port_name1  the name of the port on comp1
   * \param comp_name2  component on the other end of the connection
   * \param port_name2  the name of the port on comp2
   *
   * An endpoint is specified by the component's local name (given as
   * component_name in the call to register_component) and the name of
   * the port on that component.
   *
   * To connect an internal or relay port, use "self" as the component name.
   */
  void
  connect(const std::string &comp_name1, const std::string &port_name1,
	  const std::string &comp_name2, const std::string &port_name2);

  /*!
   * \brief disconnect endpoint_1 from endpoint_2
   *
   * \param comp_name1  component on one end of the connection
   * \param port_name1  the name of the port on comp1
   * \param comp_name2  component on the other end of the connection
   * \param port_name2  the name of the port on comp2
   *
   * An endpoint is specified by the component's local name (given as
   * component_name in the call to register_component) and the name of
   * the port on that component.
   *
   * To disconnect an internal or relay port, use "self" as the component name.
   */
  void
  disconnect(const std::string &comp_name1, const std::string &port_name1,
	     const std::string &comp_name2, const std::string &port_name2);

  /*!
   * \brief disconnect all connections to specified component
   * \param component_name component to disconnect
   */
  void
  disconnect_component(const std::string component_name);

  /*!
   * \brief disconnect all connections to all components
   */
  void
  disconnect_all();

  /*!
   * \brief Return number of connections (QA mostly)
   */
  int
  nconnections() const;

  bool
  walk_tree(mb_visitor *visitor, const std::string &path="");
  
  mb_msg_accepter_sptr
  make_accepter(const std::string port_name);

  mb_msg_queue &
  msgq() { return d_msgq; }

  //! Return full name of this block
  std::string fullname() const { return d_fullname; }

  //! Set the name of this block
  void set_fullname(const std::string &name);

  /*!
   * \brief If bound, store endpoint from the other end of the connection.
   *
   * \param port [in]  port the port that we're searching for.
   * \param ep   [out] the other end point from the matching connection.
   *
   * \returns true iff there's a matching connection.
   */
  bool
  lookup_other_endpoint(const mb_port *port, mb_endpoint *ep);


  mb_mblock *
  mblock() const { return d_mb; }

  mb_mblock *
  mblock_parent() const { return d_mb_parent; }

  mb_mblock_sptr
  component(const std::string &comp_name);


  /*
   * Our implementation methods
   */
private:
  //bool port_is_defined(pmt_t name);
  bool port_is_defined(const std::string &name);
  //bool comp_is_defined(pmt_t name);
  bool comp_is_defined(const std::string &name);

  mb_endpoint 
  check_and_resolve_endpoint(const std::string &comp_name,
			     const std::string &port_name);


  mb_port_sptr
  resolve_port(const std::string &comp_name,
	       const std::string &port_name);

  static bool
  endpoints_are_compatible(const mb_endpoint &ep0,
			   const mb_endpoint &ep1);

};


#endif /* INCLUDED_MB_MBLOCK_IMPL_H */
