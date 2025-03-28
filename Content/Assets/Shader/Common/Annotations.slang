module Annotations;

[__AttributeUsage(_AttributeTargets.Var)]
public struct DynamicUniformAttribute {};

[__AttributeUsage(_AttributeTargets.Var)]
public struct LabelAttribute {
    string name;
};