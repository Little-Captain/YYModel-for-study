//
//  NSObject+YYModel.h
//  YYModel <https://github.com/ibireme/YYModel>
//
//  Created by ibireme on 15/5/10.
//  Copyright (c) 2015 ibireme.
//
//  This source code is licensed under the MIT-style license found in the
//  LICENSE file in the root directory of this source tree.
//

#import <Foundation/Foundation.h>

// NS_ASSUME_NONNULL_BEGIN 和 NS_ASSUME_NONNULL_END 宏包裹的部分中,
// 方法参数在不明确指定的情况下都是不能传递`空`
// 如果可以传递`空`, 可以加入 nullable 关键字
// 为什么要使用这两个宏? 因为大部分参数都是不能传递`空`的, 少部分是可以传递`空`的, 为了减少人工指定的工作量, 就有必要使用这两个空定义
NS_ASSUME_NONNULL_BEGIN

/**
 提供一些 data-model 方法
 * 转换 json 到 任何对象, 或转换 任何对象 到 json
 * 通过字典设置对象的属性, 就像 KVC 一样
 * 实现对象的 NSCoding、NSCopying协议、hash方法和对象比较方法(isEqual)
 
 通过 YYModel 协议 可以定制 data-model 方法的行为
 
 示例代码:
     ********************** json 转换 *********************
     @interface YYAuthor : NSObject
     @property (nonatomic, strong) NSString *name;
     @property (nonatomic, assign) NSDate *birthday;
     @end
     @implementation YYAuthor
     @end
 
     @interface YYBook : NSObject
     @property (nonatomic, copy) NSString *name;
     @property (nonatomic, assign) NSUInteger pages;
     @property (nonatomic, strong) YYAuthor *author;
     @end
     @implementation YYBook
     @end
    
     int main() {
         // create model from json
         YYBook *book = [YYBook yy_modelWithJSON:@"{\"name\": \"Harry Potter\", \"pages\": 256, \"author\": {\"name\": \"J.K.Rowling\", \"birthday\": \"1965-07-31\" }}"];
 
         // convert model to json
         NSString *json = [book yy_modelToJSONString];
         // {"author":{"name":"J.K.Rowling","birthday":"1965-07-31T00:00:00+0000"},"name":"Harry Potter","pages":256}
     }
 
     ********************** 对象 : 编码/拷贝/哈希/比较 *********************
     @interface YYShadow :NSObject <NSCoding, NSCopying>
     @property (nonatomic, copy) NSString *name;
     @property (nonatomic, assign) CGSize size;
     @end
 
     @implementation YYShadow
     - (void)encodeWithCoder:(NSCoder *)aCoder { [self yy_modelEncodeWithCoder:aCoder]; }
     - (id)initWithCoder:(NSCoder *)aDecoder { self = [super init]; return [self yy_modelInitWithCoder:aDecoder]; }
     - (id)copyWithZone:(NSZone *)zone { return [self yy_modelCopy]; }
     - (NSUInteger)hash { return [self yy_modelHash]; }
     - (BOOL)isEqual:(id)object { return [self yy_modelIsEqual:object]; }
     @end
 
 */
@interface NSObject (YYModel)

/**
 通过 json 创建并返回一个新的接收者对象实例, 这个方法是线程安全的
 @param json  json对象: 可存在于字典、字符串、NSData对象中
 @return 创建的对象实例, 如果为 nil 就是发生了转换错误
 */
+ (nullable instancetype)yy_modelWithJSON:(id)json;

/**
 通过 NSDictionary 对象 创建并返回一个新的接收者对象实例, , 这个方法是线程安全的
 @param dictionary  创建实例的字典
 字典中任何无效的键值对都将被忽略
 
 @return 创建的对象实例, 如果为 nil 就是发生了转换错误
 
 @discussion 字典中的 key 和接收者实例的属性名相对应, 字典中的 value 将被设置为属性值
 如果 value 的类型和实例的属性不匹配, 这个方法将尝试将 value 按照如下规则进行转换:
 
     `NSString` or `NSNumber` -> c number, such as BOOL, int, long, float, NSUInteger...
     `NSString` -> NSDate, parsed with format "yyyy-MM-dd'T'HH:mm:ssZ", "yyyy-MM-dd HH:mm:ss" or "yyyy-MM-dd".
     `NSString` -> NSURL.
     `NSValue` -> struct or union, such as CGRect, CGSize, ...
     `NSString` -> SEL, Class.
 */
+ (nullable instancetype)yy_modelWithDictionary:(NSDictionary *)dictionary;

/**
 通过一个 json 对象设置 一个实例的属性
 json 中任何无效的数据都将被忽略
 @param json 用字典、字符串、NSData描述的 json 对象
 
 @return 是否设置成功
 */
- (BOOL)yy_modelSetWithJSON:(id)json;

/**
 通过一个 字典 设置 一个实例的属性
 @param dic  设置需要的字典
 字典中任何无效的键值对都将被忽略
 
 @discussion 字典的 key 对应 对象的属性名, 字典的 value 将设置为 对象的属性值.
 如果 value 的类型和属性不匹配, 这个方法将尝试将 value 按照如下规则进行转换 :
 
     `NSString`, `NSNumber` -> c number, such as BOOL, int, long, float, NSUInteger...
     `NSString` -> NSDate, parsed with format "yyyy-MM-dd'T'HH:mm:ssZ", "yyyy-MM-dd HH:mm:ss" or "yyyy-MM-dd".
     `NSString` -> NSURL.
     `NSValue` -> struct or union, such as CGRect, CGSize, ...
     `NSString` -> SEL, Class.
 
 @return Whether succeed.
 */
- (BOOL)yy_modelSetWithDictionary:(NSDictionary *)dic;

/**
 将模型转换为 json 对象
 @return A json object in `NSDictionary` or `NSArray`, or nil if an error occurs.
 json 对象用 NSDictionary、NSArray 存放, 如果为 nil 就是发生了错误
 查看 [NSJSONSerialization isValidJSONObject] 获取更多信息.
 
 @discussion 任何无效的属性都将被忽略. 如果 reciver 是 NSArray、NSDictionary、NSSet, 
 它仅仅将内部的对象转换为 json 对象
 */
- (nullable id)yy_modelToJSONObject;

/**
 通过 receiver 的属性 生成一个 json 字符串 data
 @return json 字符串 data, 如果为 nil , 表示发生了错误.
 
 @discussion 任何无效的属性都将被忽略. 如果 reciver 是 NSArray、NSDictionary、NSSet,
 它也要将内部的对象转换为 json 字符串 data
 */
- (nullable NSData *)yy_modelToJSONData;

/**
 通过 receiver 的属性 生成一个 json 字符串
 @return json 字符串, 如果为 nil , 表示发生了错误.
 
 @discussion 任何无效的属性都将被忽略. 如果 reciver 是 NSArray、NSDictionary、NSSet,
 它也要将内部的对象转换为 json 字符串
 */
- (nullable NSString *)yy_modelToJSONString;

/**
 连同属性拷贝实例
 @return 被拷贝的实例对象, 如果为 nil , 表示发生了错误.
 */
- (nullable id)yy_modelCopy;

/**
 Encode the receiver's properties to a coder.
 将 receiver 的属性 归档
 @param aCoder  归档对象.
 */
- (void)yy_modelEncodeWithCoder:(NSCoder *)aCoder;

/**
 将 receiver 的属性 解档
 @param aDecoder  解档对象.
 
 @return 自身
 */
- (id)yy_modelInitWithCoder:(NSCoder *)aDecoder;

/**
 通过 receiver 的属性生成 hash 编码.
 
 @return hash 编码.
 */
- (NSUInteger)yy_modelHash;

/**
 在对象属性的基础上, 比较两个对象是否相等
 @param model  比较的对象
 
 @return 是否相等.
 */
- (BOOL)yy_modelIsEqual:(id)model;

/**
 Description method for debugging purposes based on properties.
 调试方法, 用于把 receiver 的属性以字符串的形式返回
 @return 描述 receiver 内容的字符串.
 */
- (NSString *)yy_modelDescription;

@end



/**
 为 NSArray 提供一些 data-model 方法
 */
@interface NSArray (YYModel)

/**
 通过 json-array 创建并返回一个数组, 这个方法是线程安全的
 @param cls  数组中 模型 的类型.
 @param json  json 数组, 可以是NSArray、NSString、NSData的形式
              Example: [{"name","Mary"},{name:"Joe"}]
 
 @return 数组, 如果为 nil, 表示发生了错误.
 */
+ (nullable NSArray *)yy_modelArrayWithClass:(Class)cls json:(id)json;

@end



/**
 为 NSDictionary 提供一些 data-model 方法
 */
@interface NSDictionary (YYModel)

/**
 通过 json 创建并返回一个字典, 这个方法是线程安全的
 @param cls  字典中 value 的 模型类型.
 @param json  json 字典, 可以是NSDictionary、NSString、NSData的形式.
              Example: {"user1":{"name","Mary"}, "user2": {name:"Joe"}}
 
 @return 字典, 如果为 nil, 表示发生了错误.
 */
+ (nullable NSDictionary *)yy_modelDictionaryWithClass:(Class)cls json:(id)json;
@end



/**
 If the default model transform does not fit to your model class, implement one or
 more method in this protocol to change the default key-value transform process.
 There's no need to add '<YYModel>' to your class header.
 */
@protocol YYModel <NSObject>
@optional

/**
 Custom property mapper.
 
 @discussion If the key in JSON/Dictionary does not match to the model's property name,
 implements this method and returns the additional mapper.
 
 Example:
    
    json: 
        {
            "n":"Harry Pottery",
            "p": 256,
            "ext" : {
                "desc" : "A book written by J.K.Rowling."
            },
            "ID" : 100010
        }
 
    model:
        @interface YYBook : NSObject
        @property NSString *name;
        @property NSInteger page;
        @property NSString *desc;
        @property NSString *bookID;
        @end
        
        @implementation YYBook
        + (NSDictionary *)modelCustomPropertyMapper {
            return @{@"name"  : @"n",
                     @"page"  : @"p",
                     @"desc"  : @"ext.desc",
                     @"bookID": @[@"id", @"ID", @"book_id"]};
        }
        @end
 
 @return A custom mapper for properties.
 */
+ (nullable NSDictionary<NSString *, id> *)modelCustomPropertyMapper;

/**
 The generic class mapper for container properties.
 
 @discussion If the property is a container object, such as NSArray/NSSet/NSDictionary,
 implements this method and returns a property->class mapper, tells which kind of 
 object will be add to the array/set/dictionary.
 
  Example:
        @class YYShadow, YYBorder, YYAttachment;
 
        @interface YYAttributes
        @property NSString *name;
        @property NSArray *shadows;
        @property NSSet *borders;
        @property NSDictionary *attachments;
        @end
 
        @implementation YYAttributes
        + (NSDictionary *)modelContainerPropertyGenericClass {
            return @{@"shadows" : [YYShadow class],
                     @"borders" : YYBorder.class,
                     @"attachments" : @"YYAttachment" };
        }
        @end
 
 @return A class mapper.
 */
+ (nullable NSDictionary<NSString *, id> *)modelContainerPropertyGenericClass;

/**
 If you need to create instances of different classes during json->object transform,
 use the method to choose custom class based on dictionary data.
 
 @discussion If the model implements this method, it will be called to determine resulting class
 during `+modelWithJSON:`, `+modelWithDictionary:`, conveting object of properties of parent objects 
 (both singular and containers via `+modelContainerPropertyGenericClass`).
 
 Example:
        @class YYCircle, YYRectangle, YYLine;
 
        @implementation YYShape

        + (Class)modelCustomClassForDictionary:(NSDictionary*)dictionary {
            if (dictionary[@"radius"] != nil) {
                return [YYCircle class];
            } else if (dictionary[@"width"] != nil) {
                return [YYRectangle class];
            } else if (dictionary[@"y2"] != nil) {
                return [YYLine class];
            } else {
                return [self class];
            }
        }

        @end

 @param dictionary The json/kv dictionary.
 
 @return Class to create from this dictionary, `nil` to use current class.

 */
+ (nullable Class)modelCustomClassForDictionary:(NSDictionary *)dictionary;

/**
 All the properties in blacklist will be ignored in model transform process.
 Returns nil to ignore this feature.
 
 @return An array of property's name.
 */
+ (nullable NSArray<NSString *> *)modelPropertyBlacklist;

/**
 If a property is not in the whitelist, it will be ignored in model transform process.
 Returns nil to ignore this feature.
 
 @return An array of property's name.
 */
+ (nullable NSArray<NSString *> *)modelPropertyWhitelist;

/**
 This method's behavior is similar to `- (BOOL)modelCustomTransformFromDictionary:(NSDictionary *)dic;`, 
 but be called before the model transform.
 
 @discussion If the model implements this method, it will be called before
 `+modelWithJSON:`, `+modelWithDictionary:`, `-modelSetWithJSON:` and `-modelSetWithDictionary:`.
 If this method returns nil, the transform process will ignore this model.
 
 @param dic  The json/kv dictionary.
 
 @return Returns the modified dictionary, or nil to ignore this model.
 */
- (NSDictionary *)modelCustomWillTransformFromDictionary:(NSDictionary *)dic;

/**
 If the default json-to-model transform does not fit to your model object, implement
 this method to do additional process. You can also use this method to validate the 
 model's properties.
 
 @discussion If the model implements this method, it will be called at the end of
 `+modelWithJSON:`, `+modelWithDictionary:`, `-modelSetWithJSON:` and `-modelSetWithDictionary:`.
 If this method returns NO, the transform process will ignore this model.
 
 @param dic  The json/kv dictionary.
 
 @return Returns YES if the model is valid, or NO to ignore this model.
 */
- (BOOL)modelCustomTransformFromDictionary:(NSDictionary *)dic;

/**
 If the default model-to-json transform does not fit to your model class, implement
 this method to do additional process. You can also use this method to validate the
 json dictionary.
 
 @discussion If the model implements this method, it will be called at the end of
 `-modelToJSONObject` and `-modelToJSONString`.
 If this method returns NO, the transform process will ignore this json dictionary.
 
 @param dic  The json dictionary.
 
 @return Returns YES if the model is valid, or NO to ignore this model.
 */
- (BOOL)modelCustomTransformToDictionary:(NSMutableDictionary *)dic;

@end

NS_ASSUME_NONNULL_END
