/*
 *
 *
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#include "typeobject_generator.h"
#include "be_extern.h"

#include "utl_identifier.h"

#include "topic_keys.h"

using namespace AstTypeClassification;

bool
typeobject_generator::gen_enum(AST_Enum*, UTL_ScopedName* name,
  const std::vector<AST_EnumVal*>& contents, const char*)
{
  return true;
}

namespace {

}

bool
typeobject_generator::gen_struct(AST_Structure* node, UTL_ScopedName* name,
  const std::vector<AST_Field*>& fields, AST_Type::SIZE_TYPE, const char*)
{
  NamespaceGuard ng;

  be_global->add_include("dds/DCPS/TypeObject.h",
                         BE_GlobalData::STREAM_H);

  const std::string clazz = scoped(name);

  const std::string exp = be_global->export_macro().c_str();
  {
    const std::string decl = "const XTypes::TypeObject& getTypeObject<" + clazz + ">()";
    be_global->header_ << "template<>\n" << exp << (exp.length() ? "\n" : "")
                       << decl << ";\n";
  }
  {
    const std::string decl = "RcHandle<XTypes::TypeIdentifier> getTypeIdentifier<" + clazz + ">()";
    be_global->header_ << "template<>\n" << exp << (exp.length() ? "\n" : "")
                       << decl << ";\n";
  }

  // TODO: Pick the appropriate flags.
  // TODO: Support struct inheritance.
  be_global->impl_ <<
    "template<>\n"
    "const XTypes::TypeObject& getTypeObject<" << clazz << ">()"
    "{\n"
    "    static XTypes::TypeObject to = XTypes::TypeObject::make(\n"
    "      XTypes::MinimalTypeObject::make(\n"
    "        XTypes::MinimalStructType::make(XTypes::IS_FINAL | XTypes::IS_NESTED | XTypes::IS_AUTOID_HASH,\n"
    "          XTypes::MinimalStructHeader::make(\n"
    "            getTypeIdentifier<XTypes::TkNone>(),\n"
    "            XTypes::MinimalTypeDetail::make()\n"
    "          ),\n"
    "          XTypes::MinimalStructMemberSeq()";

  ACE_CDR::ULong member_id = 0;
  for (std::vector<AST_Field*>::const_iterator pos = fields.begin(), limit = fields.end(); pos != limit; ++pos) {
    be_global->impl_ << ".append(\n";
    be_global->impl_ << "XTypes::MinimalStructMember::make(";
    be_global->impl_ << "  XTypes::CommonStructMember::make(";
    be_global->impl_ << "    " << member_id++ << ",";
    // TODO: Set StructMemberFlags.
    be_global->impl_ << "    " << 0 << ",";
    be_global->impl_ << "    getTypeIdentifier<" << scoped((*pos)->field_type()->name()) <<  ">()";
    be_global->impl_ << "  ),";
    be_global->impl_ << "  XTypes::MinimalMemberDetail::make(\"" << (*pos)->local_name()->get_string() << "\")";
    be_global->impl_ <<  ")\n";
    be_global->impl_ << ")\n";
  }

  be_global->impl_ <<
    "        )\n"
    "      )"
    "    );\n"
    "    return to;"
    "  }\n";

  // be_global->impl_ <<
  //   "};\n\n"
  //   "template<>\n"
  //                  << decl << "\n"
  //   "{\n"
  //   "  static MetaStructImpl<" << clazz << "> msi;\n"
  //   "  return msi;\n"
  //   "}\n\n";

  return true;
}

bool
typeobject_generator::gen_typedef(AST_Typedef*, UTL_ScopedName* name,
  AST_Type* type, const char*)
{
  return true;
}

bool
typeobject_generator::gen_union(AST_Union* node, UTL_ScopedName* name,
  const std::vector<AST_UnionBranch*>& branches, AST_Type* discriminator,
  const char*)
{
  // const std::string clazz = scoped(name);
  // ContentSubscriptionGuard csg;
  // NamespaceGuard ng;

  // std::vector<AST_Field*> dummy_field_list;
  // if (!generate_typeobject(node, name, dummy_field_list, first_struct_, clazz)) {
  //   return false;
  // }

  // {
  //   Function f("gen_skip_over", "bool");
  //   f.addArg("ser", "Serializer&");
  //   f.addArg("", clazz + "*");
  //   f.endArgs();
  //   be_global->impl_ <<
  //     "  " << scoped(discriminator->name()) << " disc;\n"
  //     "  if (!(ser >> " << getWrapper("disc", discriminator, WD_INPUT) << ")) {\n"
  //     "    return false;\n"
  //     "  }\n";
  //   if (generateSwitchForUnion("disc", func, branches, discriminator, "", "", "",
  //                              false, true, false)) {
  //     be_global->impl_ <<
  //       "  return true;\n";
  //   }
  // }
  return true;
}
