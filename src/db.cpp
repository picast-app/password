#include "db.h"

DBClient::DBClient() {
  Aws::SDKOptions options;
  Aws::InitAPI(options);

  Aws::Client::ClientConfiguration clientConfig;
  client = new Aws::DynamoDB::DynamoDBClient(clientConfig);
};

DBClient::~DBClient() {
  delete client;
};

std::unique_ptr<Item> DBClient::getItem(const char* key) {
  Aws::DynamoDB::Model::GetItemRequest request;
  request.SetTableName("echo_passwords");
  request.AddKey("id", Aws::DynamoDB::Model::AttributeValue(key));

  auto result = client->GetItem(request);
  if (!result.IsSuccess()) throw result.GetError().GetMessage();
  const Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>& record = result.GetResult().GetItem();

  if (!record.size()) return nullptr;

  auto item = std::unique_ptr<Item>(new Item());
  item->id = record.at("id").GetS();
  item->hash = record.at("hash").GetS();
  return item;
};

void DBClient::putItem(const char* id, const char* hash) {
  Aws::DynamoDB::Model::PutItemRequest request;
  request.SetTableName("echo_passwords");

  request.AddItem("id", Aws::DynamoDB::Model::AttributeValue(id));
  request.AddItem("hash", Aws::DynamoDB::Model::AttributeValue(hash));

  auto result = client->PutItem(request);
  if (!result.IsSuccess()) throw result.GetError().GetMessage();
}
