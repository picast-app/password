#include "db.h"

DBClient::DBClient() {
  Aws::Client::ClientConfiguration clientConfig;
  clientConfig.connectTimeoutMs = 1000;
  clientConfig.requestTimeoutMs = 1000;
  clientConfig.httpRequestTimeoutMs = 1000;
  clientConfig.caFile = "/etc/pki/tls/certs/ca-bundle.crt"; 
  clientConfig.region = "eu-west-1";
  auto credentialsProvider = Aws::MakeShared<Aws::Auth::EnvironmentAWSCredentialsProvider>("LAMBDA_ALLOC");
  client = new Aws::DynamoDB::DynamoDBClient(credentialsProvider, clientConfig);
};

DBClient::~DBClient() {
  delete client;
};

std::unique_ptr<Item> DBClient::getItem(std::string key) {
  Aws::DynamoDB::Model::GetItemRequest request;
  request.SetTableName(table_name);
  request.AddKey("key", Aws::DynamoDB::Model::AttributeValue(key));

  auto result = client->GetItem(request);
  if (!result.IsSuccess()) throw result.GetError().GetMessage();
  const Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>& record = result.GetResult().GetItem();

  if (!record.size()) return nullptr;

  auto item = std::unique_ptr<Item>(new Item());
  item->key = record.at("key").GetS();
  item->hash = record.at("hash").GetS();
  item->id = record.at("id").GetS();
  return item;
};

void DBClient::putItem(std::string key, std::string hash, std::string id) {
  Aws::DynamoDB::Model::PutItemRequest request;
  request.SetTableName(table_name);

  request.AddItem("key", Aws::DynamoDB::Model::AttributeValue(key));
  request.AddItem("hash", Aws::DynamoDB::Model::AttributeValue(hash));
  request.AddItem("id", Aws::DynamoDB::Model::AttributeValue(id));

  auto result = client->PutItem(request);
  if (!result.IsSuccess()) throw result.GetError().GetMessage();
}

void DBClient::deleteItem(std::string key) {
  Aws::DynamoDB::Model::DeleteItemRequest request;
  request.SetTableName(table_name);
  request.AddKey("key", Aws::DynamoDB::Model::AttributeValue(key));

  auto result = client->DeleteItem(request);
  if (!result.IsSuccess()) throw result.GetError().GetMessage();
}
