from django.db import models
from django.contrib.auth.models import User
from django.utils.timezone import now

# Create your models here.


class Item(models.Model):
    id = models.IntegerField(primary_key=True)
    description = models.CharField(max_length=100, blank=False, null=False)
    price = models.FloatField(default=0.99, blank=False, null=False)

    def __str__(self):
        return self.description

    
class Package(models.Model):
    owner = models.ForeignKey(User, on_delete=models.CASCADE, related_name="packages")
    warehouse_id = models.IntegerField()
    STATUS_CHOICES = [
        ('PACKING', 'packing'),
        ('PACKED', 'packed'),
        ('LOADING', 'loading'),
        ('LOADED', 'loaded'),
        ('DELIVERING', 'delivering'),
        ('DELIVERED', 'delivered'),
    ]
    status = models.CharField(
        max_length=50, choices=STATUS_CHOICES, default='packing')
    addr_x = models.IntegerField(default=5)
    addr_y = models.IntegerField(default=5)
    # for ups connection
    ups_id = models.IntegerField()
    
class Order(models.Model):
    # customer info
    owner = models.ForeignKey(User, on_delete=models.CASCADE, related_name="orders")
    
    # order info
    id = models.AutoField(primary_key=True)
    item = models.ForeignKey(Item, on_delete=models.SET_NULL, null=True)
    item_cnt = models.IntegerField(default=1)
    package = models.ForeignKey(Package, on_delete=models.CASCADE, related_name="orders", null=True, blank=True)

    # return the total price for current order
    def total(self):
        return self.item_cnt * self.item.price

    def __str__(self):
        return "<" + str(self.item_id) + ', ' + str(self.item_cnt) + ">"
    

class Inventory(models.Model):
    item = models.ForeignKey(Item, on_delete=models.SET_NULL, null=True)
    item_cnt = models.IntegerField(default=10)
    warehouse_id = models.IntegerField()

    class Meta:
        unique_together = ["item", "warehouse_id"]
        db_table = 'inventory'
