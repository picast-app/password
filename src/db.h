#pragma once

#include <string>
#include <memory>
#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h> 
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/AttributeDefinition.h>
#include <aws/dynamodb/model/GetItemRequest.h>
#include <aws/dynamodb/model/PutItemRequest.h>
#include <aws/dynamodb/model/DeleteItemRequest.h>
#include <aws/core/platform/Environment.h>
#include <aws/core/auth/AWSCredentialsProvider.h>

struct Item {
  std::string key;
  std::string hash;
  std::string id;
};

class DBClient {
  public:
    DBClient();
    ~DBClient();

  std::unique_ptr<Item> getItem(std::string key);
  void putItem(std::string key, std::string hash, std::string id);
  void deleteItem(std::string key);

  private:
    Aws::DynamoDB::DynamoDBClient* client = nullptr;
    const std::string table_name = "echo_passwords";
};
