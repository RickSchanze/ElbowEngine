当想要增加自定义的生成器时  
对于Property，使用
```python
@property_annotation_generator
class SimpleEditorMetaGenerator(PropertyAnnotationGenerator):
    def generate_source(self, entity: ReflProperty, anno_key: str, anno_value: str) -> str:
        return f" EDITOR_META( (rttr::metadata(\"{anno_key}\", \"{anno_value}\") )"

    def match(self, entity: ReflProperty, anno_key: str, anno_value: str) -> bool:
        if anno_key in ["Label"]:
            return True
        return False
```
这里只返回可以跟在rttr后面接着调用的调用，不要加换行
