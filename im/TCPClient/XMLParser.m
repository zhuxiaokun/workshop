/*
 Erica Sadun, http://ericasadun.com
 iPhone Developer's Cookbook, 3.0 Edition
 BSD License, Use at your own risk
 */

//
//  XMLParser.m
//  Created by Erica Sadun on 4/6/09.
//

#import "XMLParser.h"


@implementation XMLParser

static XMLParser *sharedInstance = nil;

// Use just one parser instance at any time
+(XMLParser *) sharedInstance 
{
    if(!sharedInstance) {
		sharedInstance = [[self alloc] init];
    }
    return sharedInstance;
}

+(id)allocWithZone:(NSZone *)zone
{
	@synchronized(self)
	{
		if (!sharedInstance) {
			sharedInstance=[super allocWithZone:zone];
			return sharedInstance;
		}
	}
    NSAssert(!sharedInstance,@"You Cannot create second instance of XMLParser!");
	return nil;
}

// Parser returns the tree root. You may have to go down one node to the real results
- (TreeNode *) parse: (NSXMLParser *) parser
{
	//构建一个可变数组
	stack = [NSMutableArray array];
	//声明根结点
	TreeNode *root = [TreeNode treeNode];
	root.parent = nil;
	root.leafvalue = nil;
	//孩子节点
	root.children = [NSMutableArray array];
	//向数组中添加根结点
	[stack addObject:root];
	[parser setDelegate:self];
	// called to start the event-driven parse. Returns YES in the event of a successful parse, and NO in case of error.
	[parser parse];
    [parser release];

	// pop down to real root
	TreeNode *realroot = [[root children] lastObject];
	root.children = nil;
	root.parent = nil;
	root.leafvalue = nil;
	root.key = nil;
	root.atributes=nil;
	realroot.parent = nil;
	return realroot;
}

//从url里解析XML
- (TreeNode *)parseXMLFromURL: (NSURL *) url
{	
	TreeNode *results;
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    NSXMLParser *parser = [[NSXMLParser alloc] initWithContentsOfURL:url];
	results = [self parse:parser];
	[pool drain];
	return results;
}

//从NSData中解析XML
- (TreeNode *)parseXMLFromData: (NSData *) data
{	
	TreeNode *results;
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    NSXMLParser *parser = [[NSXMLParser alloc] initWithData:data];
	results = [self parse:parser];
	[pool drain];
	return results;
}



// Descend to a new element到一个新的节点
- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict
{
    if (qName) elementName = qName;
	//NSLog(@"%d",[attributeDict count]);
//	NSLog(@"%d",[[attributeDict allKeys] count]);
//	NSLog(@"%@",[attributeDict objectForKey:@"group"]);
	TreeNode *node = [TreeNode treeNode];
	node.parent = [stack lastObject];
	[(NSMutableArray *)[[stack lastObject] children] addObject:node];
	
	node.key = [NSString stringWithString:elementName];
	node.leafvalue = nil;
	node.children = [NSMutableArray array];
	node.atributes=attributeDict;
    
	[stack addObject:node];
}

// Pop after finishing element
- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName
{
	[stack removeLastObject];
}

// Reached a leaf
- (void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string
{
	if (![[stack lastObject] leafvalue])
	{
		[[stack lastObject] setLeafvalue:[NSString stringWithString:string]];
		return;
	}
	[[stack lastObject] setLeafvalue:[NSString stringWithFormat:@"%@%@", [[stack lastObject] leafvalue], string]];
}

@end



